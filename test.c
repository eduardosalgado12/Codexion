#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *processar_dados(void *arg) {
    int id = *(int *)arg;
    
    // Alocamos memória dinamicamente para o resultado que vamos devolver.
    // Se usássemos uma variável local estática, a memória seria destruída ao sair da função.
    int *resultado = malloc(sizeof(int));
    
    printf("[Thread %d] A validar dados...\n", id);
    sleep(1);

    if (id == 2) {
        printf("[Thread %d] ERRO DETETADO! A abortar via pthread_exit...\n", id);
        *resultado = -999; // Código de erro personalizado
        
        // Termina a thread IMEDIATAMENTE e devolve o ponteiro com o erro
        return ((void *)resultado); 
    }

    // Se tudo correr bem, continua o trabalho normal
    printf("[Thread %d] Trabalho concluído com sucesso.\n", id);
    *resultado = id * 100; 
    
    return (void *)resultado; // return e pthread_exit funcionam igual aqui
}

int main(void) {
    pthread_t t1, t2;
    int id1 = 1, id2 = 2;
    
    // Ponteiros para receber as respostas de cada thread
    void *retorno_t1;
    void *retorno_t2;

    pthread_create(&t1, NULL, processar_dados, &id1);
    pthread_create(&t2, NULL, processar_dados, &id2);

    // O pthread_join recebe o endereço do ponteiro para guardar lá o resultado
    pthread_join(t1, &retorno_t1);
    pthread_join(t2, &retorno_t2);

    // Converter os ponteiros void* de volta para int* para ler o resultado
    int *res1 = (int *)retorno_t1;
    int *res2 = (int *)retorno_t2;

    printf("\n--- RESULTADOS NA MAIN ---\n");
    printf("Resultado da Thread 1: %d\n", *res1);
    printf("Resultado da Thread 2: %d (Código de Erro)\n", *res2);

    // Muito importante: libertar a memória alocada dentro das threads
    free(res1);
    free(res2);

    return 0;
}
