


// estrutura de dados para os parametros de entrada:
// porque usamos long nos times?
// R: Os resultados da funcao get_time sao numeros enormes.


#ifndef CODEXION.H
#define CODEXION.H

# include <pthread.h>

typedef struct s_params
{
    int         n_coders;
    long        t_burnout;
    long        t_compile;
    long        t_debug;
    long        t_refactor;
    int         n_complies;
    long        cooldown;
    int         scheduler;
}               t_params;

//estrutura de dados para o dongle

typedef struct s_dongle
{
    pthread_mutex_t    mutex;


}               t_dongle;

//estrutura de dados para o coder

typedef struct s_coder
{
    int         id;
    pthread_t      th; // id da thread de cada coder, para trabalharem em paralelo
    t_dongle    *left; // 'e ponteiro para garantir que aponta para o dongle criado em init_dongles, guardado no array sim->dongles;
    t_dongle    *right;
    long        last_time_compile; //“O last_compile_start guarda o momento em que o coder começou a última compilação. É escrito pelo próprio coder, logo antes de compilar, e lido pelo monitor para verificar o burnout. Como as duas threads lhe acedem ao mesmo tempo, está protegido pelo state_mutex. No início, o init_sim inicializa-o com o start_time, para o prazo contar a partir do arranque.”
    int         compile_count; // numero de compilacoes atuais
    pthread_mutex_t    state_mutex;
    struct s_sim	*sim;     // É um ponteiro de volta para o estado global. A thread do coder só recebe a sua ficha, e através deste ponteiro chega aos parâmetros, ao start_time, ao stop e ao print_mutex, sem usar variáveis globais. É escrito uma vez no init e nunca muda, por isso não precisa de mutex. É struct s_sim * porque o t_sim só é definido depois do t_coder no header.”

}               t_coder;



// estrutura de dados para a simulacao

typedef struct s_sim
{
    t_params         params;
    t_coder          *coders;
    t_dongle         *dongles;
    int              stop;
    int              start_time;
    pthread_mutex_t  stop_mutex;
    pthread_mutex_t  stop_mutex; 
    pthread_t        monitor;

}              t_sim;

#endif

#include "codexion.h"


void start_threads(t_sim *sim)
{
    int i;

    i = 0;
    while( i < sim->params.n_coders)
    {
        pthread_create(&sim->coders[i].thread,NULL, coder_routine,&sim->coders.th[i]);
        i++;
    }
    pthread_create(&sim->monitor,NULL, monitor_routine,sim)
}

int main(int argc, char **argv)
{
    t_sim sim;

    if(parse_args(argc, argv,&sim.params)== 0)
    {
        fprintf(stderr, "error");
        return(1);
    }
    if(init_sim(&sim) == 0)
    {
        fprintf(stderr, "error");
        return(1);
    }
    start_threads(&sim)
}



void *coder_routine(void *arg)