#include "codexion.h"

int	heap_push(t_heap *heap, t_request req);
int	heap_pop(t_heap *heap, t_request *out);

int	main(void)
{
	t_heap		heap;
	t_request	req;
	t_request	out;
	long		values[5] = {50, 30, 80, 10, 40};
	int			i;

	heap.size = 0;
	i = 0;
	while (i < 5)
	{
		req.coder_id = i + 1;
		req.priority = values[i];
		heap_push(&heap, req);
		printf("push %ld -> topo: %ld (size %d)\n",
			values[i], heap.items[0].priority, heap.size);
		i++;
	}
	while (heap_pop(&heap, &out))
		printf("pop: %ld (coder %d)\n", out.priority, out.coder_id);
	return (0);
}
