/**
 \file
 \author Marco Barbone
 Si dichiara il contenuto di questo file in ogni sua parte è interamente a cura dell'autore

 */

#include "declaration.h"

int check;

/* funzione per stampare la matrice in mutua esclusione, in caso di richiesta di dump */
int print_mutex_planet(FILE *dump){
    errno=0;
    pthread_mutex_lock(&job);
    ERROR("ha fallito una lock nel print mutex planet");
    errno=0;
    while (done<wator->nwork){ pthread_cond_wait(&idle, &job);
        ERROR("ha fallito una wait nel print mutex planet")
    }
    if(print_planet(dump, wator->plan)==-1){
        /* in caso di errore chiudo l'esecuzione */
        end=1;
        perror("fallita la print planet sul file di dump\n");
        return 1;
    }
    errno=0;
    pthread_cond_broadcast(&idle);
    ERROR("ha fallito una broadcast nel print mutex planet ")
    errno=0;
    pthread_mutex_unlock(&job);
    ERROR("ha fallito una unlock nel print mutex planet");
    return 0;
}

/* procedura per settare il flag di dump, in modo da sospendere gli altri thread e acquisire la lock sulla matrice condivisa */
int set_dump(){
    errno=0;
    pthread_mutex_lock(&set_dump_flag);
    ERROR("ha fallito una lock nel set_dump");
    dump_flag=1;
    errno=0;
    pthread_mutex_unlock(&set_dump_flag);
    ERROR("ha fallito una unlock set_dump");
    return 0;
}

/* procedura per resettare il flag di dump, in modo da riattuvare gli altri thread e rilascare la lock sulla matrice condivisa */

int reset_dump(){
    errno=0;
    pthread_mutex_lock(&set_dump_flag);
    ERROR("ha fallito una lock reset_dump")
    errno=0;
    dump_flag=0;
    pthread_cond_signal(&wait_dump);
    ERROR("ha fallito una signal rset_dump")
    errno=0;
    pthread_mutex_unlock(&set_dump_flag);
    ERROR("ha fallito una unlock reset_dump")
    return 0;
}

/* main del thread handler, viene creato, attende un segnale, lo gestise, se necessario termina, oppure fa il dump della matrice */
void *handler(void *l){
    FILE *dump = NULL;
    printf("handler pronto\n");
loop:
    check=sigwait(&set, &sig);
    if (check) {
        end=1;
        perror("la sigwait ha fallito\n");
        return (void*) 1;
    }
    switch (sig) {
        case SIGINT:
            end=1;
            fprintf(stderr, "terminazione gentile wator\n");
            return (void *) 0;
         case SIGPIPE:
            end=1;
            fprintf(stderr, "errore:il visualizer è terminato in modo scorretto\n");
            return (void *) 0;
        case SIGTERM:
            end=1;
            fprintf(stderr, "chiudo l'esecuzione\n");
            return (void *) 0;
        case SIGUSR1:
            dump=fopen("wator.check","w");
            if (dump==NULL) {
                end=1;
                perror("fallita l'apertura del file di dump\n");
                break;
            }
            if(set_dump()){
                end=1;
                return (void *) 1;
            }
            if (print_mutex_planet(dump)) break;
            if(reset_dump()){
                end=1;
                return (void *) 1;
            }
            fclose(dump);
            printf("SIGUSR1 ricevuta e gestita\n");
            goto loop;
        default: goto loop;
    }
    return (void *) 0;
}
