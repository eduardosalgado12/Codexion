O trabalho tem dois objetivos: o que o programa tem de fazer e o que tu tens de aprender.

O que o programa tem de fazer

Simular coders à volta de uma mesa que partilham dongles, de forma a que:

ninguém entre em burnout, se os tempos o permitirem;
nunca dois coders usem o mesmo dongle ao mesmo tempo;
o programa nunca bloqueie (sem deadlock);
o acesso seja justo, seguindo o scheduler (fifo ou edf);
se alguém entrar em burnout, isso seja detetado em menos de 10 ms e a simulação pare;
os logs saiam limpos, uma mensagem por linha.

A simulação acaba quando um coder entra em burnout ou quando todos compilaram number_of_compiles_required vezes.

O que tens de aprender

A história dos coders é só um pretexto. O verdadeiro objetivo é dominares programação concorrente, isto é, várias coisas a acontecer ao mesmo tempo:

Conceito	Onde aparece
Threads	cada coder e o monitor
Mutexes	proteger dongles, estado, logs
Condition variables	esperar por um dongle sem gastar CPU
Data races	evitá-las em tudo o que é partilhado
Deadlock	evitar que os coders fiquem todos à espera uns dos outros
Starvation	evitar que um coder nunca consiga os dongles
Scheduling	decidir quem é servido primeiro (fifo/edf)
Heap	estrutura de dados para a fila de prioridade
Tempo preciso	medir ms com gettimeofday

Estes problemas aparecem em sistemas reais: servidores web, bases de dados, sistemas operativos, jogos. Em todos eles, vários “trabalhadores” disputam recursos limitados.

No fim, deves conseguir explicar a um colega porque é que o teu programa nunca bloqueia e nunca corrompe dados. É isso que vai ser avaliado.

https://www.dei.isep.ipp.pt/~orlando/so2/threads.htm