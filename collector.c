
/**
 \file
 \author Marco Barbone
 Si dichiara il contenuto di questo file in ogni sua parte è interamente a cura dell'autore

 */


#include "declaration.h"

#define CHECK(x) if(errno){perror(x); goto vis_error;}

int sa,number_char;
char *matrix = NULL;
int ready_write=0;
pthread_mutex_t flag_write = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t wait_write = PTHREAD_COND_INITIALIZER;

/* main del thread writer, questo thread si occupa di comunicare con il visualizer */
void *writer(void *l){
    errno=0;
    while (!end) {
        errno=0;
        /* controllo se è possibile scrivere */
        pthread_mutex_lock(&flag_write);
        if (errno) {
            end=1;
            perror("ha fallito una lock nel collector");
            return (void*) 1;
        }
        while (!ready_write) {
            pthread_cond_wait(&wait_write, &flag_write);
            if (errno) {
                perror("ha fallito una wait nel collector");
                end=1;
                return (void *) 1;
            }
        }
        errno=0;
        /* spedisco al visualizer */
        write(sa, &end, sizeof(end));
        write(sa, matrix, sizeof(char)*number_char);
        if (errno){
            end=1;
            perror("write matrix fallita");
            return (void*) 1;
        }
        /* imposto a free lo stato di scrittura */
        ready_write=0;
        pthread_cond_signal(&wait_write);
        if (errno) {
            perror("ha fallito una signal nel collector");
            end=1;
            return (void *) 1;
        }
        pthread_mutex_unlock(&flag_write);
        if (errno) {
            perror("ha fallito una unlock nel collector");
            end=1;
            return (void *) 1;
        }
    }
    return (void*) 0;
}

/* procedura per testare il flag di dump, se è uguale ad 1 c'è un dump in corso, il collector aspetta di avere via libera e continua la sua esecuzione */
void test_flag(){
    errno=0;
    pthread_mutex_lock(&set_dump_flag);
    while (dump_flag==1) {
        pthread_cond_wait(&wait_dump, &set_dump_flag);
    }
    pthread_mutex_unlock(&set_dump_flag);
    if (errno) {
        perror("test flag ha fallito");
        end=1;
    }
}

/* procedura per riattivare i workers dopo avere aggiornato i chronon */
void activate_workers(){
    errno=0;
    pthread_mutex_lock(&job);
    while (done<wator->nwork){
        pthread_cond_wait(&idle, &job);
    }
    assigned=0;
    done=0;
    pthread_cond_broadcast(&idle);
    pthread_mutex_unlock(&job);
    if (errno) {
        end=1;
        perror("activate workers ha fallito");
    }
}

/* main del thread collector, questo thread crea la socket, bufferizza la matrice, codificandola in bit per risparmiare sui byte trasmessi, riattiva i worker, passa la matrice di bit ad un thread che si occupa di spedirla al visualizer*/
void *collector(void *l){
    int i,j,count=0,index=0,resto;
    char tmp=0;
    pthread_t writer_tid;
    /*informazioni necessarie per conoscere quanti byte trasmettere */
    resto=LCOL*LROW;
    resto=resto%4;
    errno=0;
    /* crazione della socket */
    sa=inizialize_socket();
    if (sa==-1) {
        fprintf(stderr, "fallita l'apertura della socket\n");
    }
    /* connessione al visualizer */
    if (connect_socket(sa)==EXIT_FAILURE) {
        return (void*) EXIT_FAILURE;
    }
    /* mando il numero di righe e di colonne al visualizer per permettergli di preparare le sue strutture dati in attesa di mandargli la matrice */
    write(sa, &LROW, sizeof(LROW));
    write(sa, &LCOL, sizeof(LCOL));
    CHECK("fallite le write dei parametri")
    /* calcolo il numero di char da mandare al visualizer */
    number_char=(LCOL*LROW)>>2;
    if (resto) number_char++;
    /* alloco la matrice di buffer */
    matrix= calloc (number_char*sizeof(char),sizeof(char));
    if (matrix==NULL) {
        goto vis_error;
    }
    /* creazione del thread che si occupa di mandare i dati al visualizer */
    errno=0;
    pthread_create(&writer_tid, 0, writer, NULL);
    CHECK("fallita la creazione del thread writer")
    while (!end) {
        int k=0;
        /* controllo che non sia in corso un dump della matrice */
        test_flag();
        /* se non è il momento di stampare riattivo i workers e salto al ciclo successivo */
        if (wator->chronon!=count && !end) {
            count++;
            activate_workers();
            continue;
        }
        /* caso in cui devo stampare */
        count=0;
        /* controllo che i workers abbiano finito*/
        errno=0;
        pthread_mutex_lock(&job);
        while (done<wator->nwork){
            pthread_cond_wait(&idle, &job);
            }
        CHECK("fallito il controllo che i workers hanno temrinato")
        errno=0;
        /* controllo che il thread abbia finito di spedire al visualizer */
        pthread_mutex_lock(&flag_write);
        while (ready_write) {
            pthread_cond_wait(&wait_write, &flag_write);
        }
        CHECK("fallito il controllo che il writers abbia terminato")
        errno=0;
        /* creo la matrice di bit da mandare al visulizer */
        for (i=0; i<LROW; i++) {
            for (j=0; j<LCOL; j++) {
                tmp+=LPLANET[i][j];
                if (index==3) {
                    matrix[k]=tmp;
                    k++;
                    index=0;
                    tmp=0;
                } else {
                    tmp<<=2;
                    index++;
                }
            }
        }
        if (resto) matrix[k+1]=tmp;
        /* imposto il flag di scrittura ad 1 */
        ready_write=1;
        /* riattivo il thread che spedisce al visualizer */
        pthread_cond_signal(&wait_write);
        pthread_mutex_unlock(&flag_write);
        CHECK("fallita la riattivazione del thread writer")
        errno=0;
        /* riattivo i workers */
        done=0;
        assigned=0;
        pthread_cond_broadcast(&idle);
        pthread_mutex_unlock(&job);
        CHECK("fallita la riattivazione dei workers")
        errno=0;
    }
    /* dealloco le strutture dati e termino il collector */
    CHECK("fallita la deallocazione del thread writer")
    free(matrix);
    close(sa);
    printf("collector terminato\n");
    return (void *) EXIT_SUCCESS;
    
vis_error:
    if (matrix!=NULL) {
        free(matrix);
    }
    end=1;
    perror("errore nel collector");
    return (void*)EXIT_FAILURE;
}
