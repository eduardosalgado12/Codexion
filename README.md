*This project has been created as part of the 42 curriculum by edsalgad.*

# Codexion

## Description

Codexion is a concurrency simulation written in C with POSIX threads.

Several **coders** sit around a circular table. Between every two neighbours there is
one **USB dongle**, so there are as many dongles as coders. To **compile**, a coder needs
the two dongles next to them at the same time. After compiling, the coder puts both
dongles back, **debugs**, **refactors**, and then tries to compile again.

The rules of the simulation:

- a dongle can only be held by one coder at a time;
- after being released, a dongle stays unavailable for `dongle_cooldown` milliseconds;
- when several coders wait for the same dongle, the dongle is granted according to a
  **scheduler**: `fifo` (first come, first served) or `edf` (earliest deadline first);
- if a coder does not start compiling within `time_to_burnout` milliseconds since the
  start of their last compilation (or since the start of the simulation), they **burn
  out** and the simulation stops;
- if every coder has compiled at least `number_of_compiles_required` times, the
  simulation stops.

The goal of the project is to practise the core problems of concurrent programming:
sharing resources between threads without data races, avoiding deadlock and
starvation, scheduling access fairly, and detecting a timeout precisely.

Each coder is a thread. A separate **monitor** thread watches every coder, detects
burnout and decides when the simulation ends.

## Instructions

### Compilation

```sh
make        # builds ./codexion
make clean  # removes the object files
make fclean # removes the object files and the binary
make re     # rebuilds everything
```

The project compiles with `cc -Wall -Wextra -Werror -pthread`.

### Usage

```sh
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

| Argument | Meaning |
|---|---|
| `number_of_coders` | number of coders, and also of dongles |
| `time_to_burnout` | max time (ms) between the start of two compilations |
| `time_to_compile` | time (ms) spent compiling, holding two dongles |
| `time_to_debug` | time (ms) spent debugging |
| `time_to_refactor` | time (ms) spent refactoring |
| `number_of_compiles_required` | the simulation stops when every coder has compiled this many times |
| `dongle_cooldown` | time (ms) a released dongle stays unavailable |
| `scheduler` | `fifo` or `edf` |

All arguments are mandatory. The program prints `Error: invalid arguments` and exits
with status 1 if:

- there are not exactly 8 arguments;
- a numeric argument is empty, negative, not an integer, or larger than `INT_MAX`;
- the scheduler is anything other than exactly `fifo` or `edf`;
- `number_of_coders` is 0.

A value of 0 is accepted for the times and for the cooldown (a cooldown of 0 means a
dongle is available again immediately).

### Examples

```sh
./codexion 1 800 200 200 200 10 0 fifo      # one dongle only: the coder must burn out at ~800
./codexion 5 2000 200 200 200 10 0 fifo     # no burnout, stops after 10 compiles each
./codexion 5 2000 200 200 200 7 0 edf       # same with edf
./codexion 5 500 200 200 200 10 0 fifo      # infeasible (cycle = 600 ms > 500): burnout at ~500
./codexion 5 3000 200 200 200 10 400 fifo   # cooldown: no burnout
```

### Output

```
timestamp_in_ms X has taken a dongle
timestamp_in_ms X is compiling
timestamp_in_ms X is debugging
timestamp_in_ms X is refactoring
timestamp_in_ms X burned out
```

The timestamp is the number of milliseconds since the start of the simulation.

## Project structure

| File | Content |
|---|---|
| `include/codexion.h` | data structures and prototypes |
| `src/main.c` | entry point, creation and joining of the threads |
| `src/utils_main.c` | argument parsing and cleanup |
| `src/init.c` | allocation and initialisation of coders, dongles and mutexes |
| `src/coder.c` | the coder thread (`coder_routine`) and the single-coder case |
| `src/dongle.c` | requesting, acquiring and releasing dongles |
| `src/heap.c` | the priority queue (binary min-heap) |
| `src/monitor.c` | the monitor thread (burnout detection and end of simulation) |
| `src/utils.c` | time, logging and the stop flag |

## Scheduling

Each dongle owns a **priority queue**, implemented as a binary min-heap
(`heap_push` / `heap_pop`, no library container). A request contains the coder id, a
priority and an arrival ticket. The request with the smallest priority is at the top.

| Scheduler | Priority of a request |
|---|---|
| `fifo` | the arrival ticket of the request on that dongle |
| `edf` | `last_compile_start + time_to_burnout` (the coder's deadline) |

**Tie-breaking rule:** two requests are compared by priority first, then by arrival
ticket, then by coder id. The order is therefore always strict and deterministic, also
for equal EDF deadlines (which happens at the start of every simulation, when all coders
have the same deadline).

A coder may take its dongles only when, for **both** dongles:

1. the dongle is not in use;
2. the coder's request is at the top of that dongle's queue;
3. the cooldown has elapsed (`now >= released_at + dongle_cooldown`).

## Blocking cases handled

### Deadlock (Coffman's conditions)

A deadlock requires the four Coffman conditions at the same time: mutual exclusion, hold
and wait, no preemption and circular wait. Mutual exclusion and no preemption are part of
the rules (a dongle has one owner and cannot be taken away), so the solution breaks the
other two.

- **Hold and wait (dongles):** a coder never holds one dongle while waiting for the
  other. It enters both queues at the same time and waits with empty hands; it only takes
  the two dongles together, once both are ready for it.
- **Circular wait (mutexes):** to check or update its two dongles, a coder locks their two
  mutexes. The mutexes are always locked in the same global order (the order of the
  dongles in the array: `lock_pair` compares the two addresses). In practice every coder
  locks its left dongle first, except the last one, which locks dongle 0 first. Because
  all threads follow the same order, a cycle of threads each holding one mutex and waiting
  for the next is impossible.
- **Circular wait (global mutexes):** `print_mutex` is always locked before `stop_mutex`,
  both in the logging functions and in the monitor.
- **No mutex is ever held while sleeping or compiling.** Mutexes are only held for a few
  instructions, so no thread can block another for a long time.

### Starvation

- **Fair queues:** each dongle serves requests strictly according to the scheduler. A
  coder that has requested a dongle cannot be overtaken on that dongle by a lower-priority
  request.
- **No overtaking through the second dongle:** because a coder enters the queues of both
  dongles at the same instant (with both mutexes locked), its position is fair in both
  queues. In an earlier version, a coder joined the second queue only after obtaining the
  first dongle and could be overtaken again and again.
- **Progress:** the requests are totally ordered (priority, ticket, id), so the coder with
  the highest priority among all waiting coders is at the top of both of its queues and
  is served as soon as its dongles are free and cooled down.
- **Convoy effect:** if all coders request in the order 1, 2, 3, ..., each coder ends up
  behind its left neighbour and only one coder compiles at a time. To avoid this, even
  coders wait 1 ms before their first request, so the arrival order becomes 1, 3, 5, ...,
  2, 4, ... and several non-neighbouring coders compile in parallel from the start. This
  does not break FIFO: requests are still served in arrival order.

### Cooldown

When a coder releases its dongles, `release_dongles` stores the current time in
`released_at` for both dongles. A dongle is considered ready only if
`now >= released_at + dongle_cooldown`. Both the write and the read are done with the
dongle's mutex locked.

### Precise burnout detection

The monitor thread checks every coder, then sleeps 1 ms. A coder burns out when
`now - last_compile_start > time_to_burnout`. At the start, `last_compile_start` is set to
the start time of the simulation, so the deadline of a coder that has not compiled yet is
counted from the beginning. With a 1 ms period, the burnout message is printed well within
10 ms of the deadline.

### Single coder

With one coder there is only one dongle (`left` and `right` point to the same dongle). This
case is handled separately: the coder takes the dongle, logs it, waits until its burnout
and returns. The monitor prints the burnout. Without this special case the coder would try
to lock the same mutex twice and block forever.

### Log serialization

Every message is printed with `print_mutex` locked, so two messages can never be mixed on
the same line. The two `has taken a dongle` lines and the `is compiling` line are printed
inside a single critical section, so they always appear together for the same coder.
Before printing, the logging functions check the stop flag inside the same critical
section, so nothing can be printed after `burned out`.

### End of the simulation

When a coder burns out, or when every coder has compiled enough times, the monitor sets the
stop flag. Each coder checks it at the beginning of every cycle and leaves its loop. The
main thread joins every thread and only then destroys the mutexes and frees the memory.

## Thread synchronization mechanisms

### Primitives

The solution uses **`pthread_mutex_t`** only. No condition variable is used for waiting
(see below).

| Mutex | Count | Protects | Used by |
|---|---|---|---|
| `dongle_mutex` | one per dongle | `in_use`, `released_at`, the dongle's queue and ticket counter | the two neighbouring coders |
| `state_mutex` | one per coder | `last_compile_start`, `compile_count` | the coder (writes) and the monitor (reads) |
| `stop_mutex` | one | the `stop` flag | the monitor (writes) and every coder (reads) |
| `print_mutex` | one | standard output | every coder and the monitor |

Data written before the threads are created and only read afterwards (the parameters,
`start_time`, the `left`/`right`/`sim` pointers) is not protected, because concurrent reads
cannot race.

### Waiting for dongles

A coder waiting for its dongles has to observe **two** dongles, protected by **two**
different mutexes. `pthread_cond_wait` can only release one mutex while sleeping, which
would leave the other dongle locked and block a neighbour. Instead, the coder uses a
short polling loop: it checks both dongles with both mutexes locked; if it is not ready,
it unlocks both, sleeps 0.5 ms with `usleep`, and checks again. The coder never holds a
mutex or a dongle while sleeping, and the CPU usage stays low.

### How race conditions are prevented

- **Taking a dongle (check then act):** checking `in_use == 0` and setting `in_use = 1` are
  done without ever unlocking the dongle's mutex in between. A neighbour trying at the same
  time waits for the mutex and then sees the dongle as busy, so a dongle can never be
  duplicated.
- **Reading a shared value:** shared values are copied into a local variable inside the
  critical section and used after the unlock. For example, `is_stopped` locks
  `stop_mutex`, copies `stop`, unlocks, and returns the copy; the monitor does the same with
  `last_compile_start` and `compile_count`.
- **Logging after the end:** the monitor sets `stop` and prints `burned out` while holding
  `print_mutex`; the coders check `stop` while holding the same mutex before printing.

### Communication between coders and the monitor

Threads never call each other; they communicate through shared state protected by mutexes:

- coder → monitor: before compiling, the coder writes `last_compile_start`; after
  compiling, it increments `compile_count` (both under its `state_mutex`). The monitor reads
  them under the same mutex.
- monitor → coders: the monitor sets `stop` under `stop_mutex`; coders read it through
  `is_stopped`.

### Verification

The program was checked with `valgrind` (no leaks), `valgrind --tool=helgrind`,
`valgrind --tool=drd` and a build with `-fsanitize=thread` (no data race, no lock-order
inversion).

## Known limitations

- After the simulation stops, coders finish the phase they are in (for example a
  `usleep` of `time_to_compile`) before leaving, so the program may take a short time to
  exit. Nothing is printed during that time.
- With a large cooldown and very tight timings, the result can depend on the OS scheduler.
  For example, with 5 coders and a cooldown of 800 ms, at most 2 coders compile per
  1000 ms round, so some coders wait almost exactly 3000 ms between two compilations.

## Resources

### References

- POSIX threads manual pages: `man pthread_create`, `man pthread_mutex_lock`,
  `man pthread_cond_wait`, `man gettimeofday`.
- CodeVault, *Unix Threads in C* (YouTube playlist): threads, mutexes, condition variables.

### Use of AI

I used an AI assistant (Claude) as a tutor during the project:

- **Concepts:** explanations of threads, mutexes, condition variables, race conditions,
  deadlock and Coffman's conditions, binary heaps, and how `pthread_cond_timedwait` works.
- **Design discussions:** how to structure the data (`t_sim`, `t_coder`, `t_dongle`), how to
  implement FIFO/EDF with one heap, and, after the evaluation tests showed burnouts with
  large cooldowns, how to remove hold-and-wait by requesting both dongles at once and how
  to avoid the convoy effect.
- **Code review and debugging:** reviews of my functions (norm, logic errors such as `&&`
  instead of `||`, wrong arguments), and help reading `-fsanitize=address`,
  `-fsanitize=thread`, `valgrind` and `helgrind` reports.
- **Documentation:** a first draft of this README, which I reviewed.    