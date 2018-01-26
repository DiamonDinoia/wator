/**
 \file
 \author Marco Barbone
 Si dichiara il contenuto di questo file in ogni sua parte è interamente a cura dell'autore

 */


#include "wator.h"
#include "rules.h"


/** questa funzione ordina (con complessità lineare) parzialmente l'array di struct motionndo a sinistra tutti gli elementi uguali al char c e restituisce il numero di ocorrenze di c
 
 \param a[] array di struct di tipo motion da ordinare
 \param c parametro di aggiornamento
 
 \return numero di occorrenze di c nell'array
 */

int partialsort(motion a[], char c){
    motion tmp;
    int i=0,j=3;
    while (i<j){
        if (a[i].val==c) i++;
        else {
            while (j>i) {
                if (a[j].val==c) {
                    tmp=a[i];
                    a[i]=a[j];
                    a[j]=tmp;
                    break;
                } else j--;
                
            }
        }
    }
    return i;
}

/** questa funzione inizializza le struct di tipo "motion" in un array
    \param pw la struct wator
    \param (x,y) coordinate dell'elemento da aggiornare
    \param motion[] l'array di struct da inizializzare
 */

void inizialize_motion(wator_t* pw, int x, int y, motion move[]){
/** calcolo le possibili posizioni in cui motionre l'elemeto */
    int sinistra=y-1, sopra=x-1, destra=y+1, sotto=x+1;
    char left, right, up, down;
/** controllo gli estremi poichè il pianeta è sferico */
    if(sinistra==-1) sinistra=COL-1;
    if(sopra==-1) sopra=ROW-1;
    if(destra==COL) destra=0;
    if(sotto==ROW) sotto=0;
    left=cell_to_char(PLANET[x][sinistra]);
    right=cell_to_char(PLANET[x][destra]);
    up=cell_to_char(PLANET[sopra][y]);
    down=cell_to_char(PLANET[sotto][y]);
/** questi assegnamenti servono a mantenere le infomazioni sulle possibli posizioni anche in caso di riordinamento dell'array */
    move[0].val=left;
    move[1].val=up;
    move[2].val=right;
    move[3].val=down;
    move[0].pos='L';
    move[1].pos='U';
    move[2].pos='R';
    move[3].pos='D';
    move[0].point=sinistra;
    move[1].point=sopra;
    move[2].point=destra;
    move[3].point=sotto;
}

/** questa procedura aggiorna i puntatori k,l con le nuove coordinate dell'elemento
 \param (x,y) coordinate iniziali dell'elemento
 \param (k,l) puntatori da aggiornare con le nuove coordinate
 \param random indice calcolato casualmente utilizzato per accedere all'array motion per estrarne le nuove coordinate
 \param motion array di struct di tipo motion che contiene le informazione necessarie a calcolare le nuove coordinate
 */

void new_coordinate(int x, int y, int *k, int *l, int random, motion move[]){
    switch (move[random].pos){
        case 'L':
            *k=x;
            *l=move[random].point;
            break;
        case 'U':
            *k=move[random].point;
            *l=y;
            break;
        case 'R':
            *k=x;
            *l=move[random].point;
            break;
        case 'D':
            *k=move[random].point;
            *l=y;
            break;
    }
}
