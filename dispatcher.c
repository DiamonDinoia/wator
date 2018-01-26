/**
 \file
 \author Marco Barbone
 Si dichiara il contenuto di questo file in ogni sua parte è interamente a cura dell'autore

 */


#include "declaration.h"

/* main del thread dispathcer questo thread calcola i lavori per i worker, li inserisce in un array condiviso,
 ogni worker accede all'array e prende un lavoro, l'ordine con cui vengono presi i lavori è determinato dallo scheduling */
void *dispatcher(void *l){
    /* calcolo la dimensione ottimale dei lavori */
    int tmp = wator->plan->nrow/wator->nwork;
    int i,resto,j;
    /* controllo che non siano rimasti lavori */
    resto=LROW%wator->nwork;
    /* questa lock è superflua, gli altri thread non dovrebbero essere ancora stati creati */
    errno=0;
    pthread_mutex_lock(&job);
    if (errno) {
        perror("ha fallito la lock del dispatcher");
        end=1;
        return (void *) 1;
    }
    /* inserisco i lavori nell'array */
    works[0].first=0;
    works[0].last=tmp-1;
    for (i=1; i<wator->nwork && i<wator->plan->nrow; i++) {
        works[i].first=works[i-1].last+1;
        works[i].last=works[i].first+tmp-1;
    }
    i=0;
    while (resto>0) {
        works[i].last++;
        for (j=i+1; j<wator->nwork && j<wator->plan->nrow; j++) {
            works[j].first=works[j-1].last+1;
            works[j].last++;
        }
        i++;
        resto--;
    }
    /* rilascio la lock e termino */
    errno=0;
    pthread_mutex_unlock(&job);
    if (errno) {
        perror("ha fallito la unlock nel dispatcher");
        end=1;
        return (void *) 1;
    }
    return (void *) 0;
}
