/**
 \file
 \author Marco Barbone
 Si dichiara il contenuto di questo file in ogni sua parte è interamente a cura dell'autore

 */




#include "declaration.h"

#define WORKERR(x) if(errno){end=1; perror(x);}

/* acquisisco la mutua esclusione sul bordo della sottomatrice */
void shared_acquire(int myjob){
    errno=0;
    pthread_mutex_lock(&share[myjob]);
    while (working[myjob]==1) {
        pthread_cond_wait(&waiting_wid[myjob], &share[myjob]);
    }
    working[myjob]=1;
    pthread_mutex_unlock(&share[myjob]);
    if (errno) {
        end=1;
        perror("shared_acquire ha fallito");
    }
}

/* rilascio la mutua esclusione sul bordo della sottomatrice */
void shared_relase(int myjob){
    errno=0;
    pthread_mutex_lock(&share[myjob]);
    working[myjob]=0;
    pthread_cond_signal(&waiting_wid[myjob]);
    pthread_mutex_unlock(&share[myjob]);
    if (errno) {
        end=1;
        perror("shared relase ha fallito");
    }
}

/* funzione per aggiornare una posizione della matrice */
int update(int x, int y,int *k, int *l){
    switch (cell_to_char(LPLANET[x][y])) {
        case 'S':
            /** controllo se ho già aggiornato uno squalo altrimenti vedo se è vivo se si lo aggiorno */
            if(LUPDATE[x][y]==0 && shark_rule2(wator, x, y, k, l)==ALIVE)
                shark_rule1 (wator ,x , y, k, l);
            break;
        case 'F':
            /** controllo se ho già aggiornato il pesce */
            if(LUPDATE[x][y]==0){
                fish_rule4(wator, x, y, k, l);
                fish_rule3(wator, x, y, k, l);
            break;
            }
    }
    return 0;
}
/* funzione per aggiornare una riga della matrice */
int update_line(int i, int *k, int *l){
    int j;
    for (j=0; j<LCOL; j++) {
        if (LPLANET[i][j]!=WATER) {
            if(update(i, j, k, l)==-1)return -1;
        }
    }
    return 0;
}

/* main del thread worker, il quale prende un lavoro, se necessario acquista la mutua esclusione e aggiorna la sua sottomatrice */
void *worker(void *unused){
    int myjob=-1,first,last,i,j,flag=1;
    char tmp[10];
    FILE* f;
    int *k = malloc(sizeof(int));
    int *l = malloc(sizeof(int));
    if (k==NULL || l==NULL) {
        fprintf(stderr,"error worker failed thread");
        pthread_exit(NULL);
    }
    
    while (!end) {
        errno=0;
        /* prendo un lavoro dalla coda condivisa */
        pthread_mutex_lock(&job);
        while(assigned>=wator->nwork) pthread_cond_wait(&idle, &job);
        myjob=assigned;
        assigned++;
        pthread_mutex_unlock(&job);
        WORKERR("fallita l'accquisizione di un lavoro")
        errno=0;
        first=works[myjob].first;
        last=works[myjob].last;
        /* aggiorno la mia sottomatrice */
        for (i=first; i<=last; i++) {
            /* aggiorno la sezione condivisa in alto */
            if (i==first || i==first+1) {
                shared_acquire(myjob);
                if (update_line(i, k, l)==-1) {
                    fprintf(stderr,"update failed thread %d\n",myjob);
                    return (void *) 1;
                }
                shared_relase(myjob);
            }
            /* aggiorno la sezione condivisa in basso */
            if (i==last || i==last-1) {
                shared_acquire((myjob+1)%wator->nwork);
                if (update_line(i, k, l)==-1) {
                    fprintf(stderr,"update failed thread %d\n",myjob);
                    return (void *) 1;
                }
                shared_relase((myjob+1)%wator->nwork);
            }
            /* aggiorno la sezione non condivisa */
            if (update_line(i, k, l)==-1) {
                fprintf(stderr,"update failed thread %d\n",myjob);
                return (void *) 1;
            }
        }
        /*riazzero i flag di aggiornamento */
        for (i=first; i<=last; i++)
            for (j=0; j<LCOL; j++) {
                LUPDATE[i][j]=0;
        }
        /* notifico al collector che ho terminato */
        errno=0;
        pthread_mutex_lock(&job);
        done++;
        if (done==wator->nwork) {
            pthread_cond_broadcast(&idle);
        }
        pthread_mutex_unlock(&job);
        WORKERR("fallita la segnalazione al collector")
        errno=0;
        }
    
 /* se è la prima volta che vengo mandato in esecuzione, creo il file con nome del worker prima di terminare */
    if (flag) {
        char namewid[25]={"wator_worker_"};
        sprintf(tmp, "%d",myjob);
        strcat(namewid, tmp);
        f=fopen(namewid, "w");
        if (f==NULL) {
            fprintf(stderr,"fopen failed thread %d\n",myjob);
            pthread_exit(NULL);
        }
        fclose(f);
        flag=0;
    }
        free(k);
        free(l);
    
    printf("worker terminato\n");
    return (void *) 0;
    
}
