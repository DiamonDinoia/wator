/**
 \file
 \author Marco Barbone
 Si dichiara il contenuto di questo file in ogni sua parte è interamente a cura dell'autore
 \brief header che contiene alcune funzioni che utilizzate per aggiornare il pianeta
 */

#ifndef Progetto_SOL_rules_h
#define Progetto_SOL_rules_h

#include "wator.h"

/** struttura che contiene le informazioni sulla possibile nuova coordinata */
typedef struct motion{
/** informazione sulla direzione dello spostamento 
 possibili valori:
    L -> se mi sposto a sinistra
    U -> se mi sposto in alto
    R -> se mi sposto a destra
    D -> se mi sposto in basso
    */
    char pos;
/** informazione sul contenuto nella cella in cui mi sto spostando
 possibili valori:
    F -> se c'è un pesce
    S -> se c'è uno squalo
    W -> se è presente dell'acqua
 */
    char val;
/** valore della coordinata */
    int point;
} motion;

/** questa funzione inizializza le struct di tipo motion in un array
 \param pw la struct wator
 \param (x,y) coordinate dell'elemento da aggiornare
 \param move[] l'array di struct da inizializzare
 */

void inizialize_motion(wator_t *pw, int x, int y, motion move[]);


/** questa funzione ordina (con complessità lineare) parzialmente l'array di struct spostando a sinistra tutti gli elementi uguali al char c e restituisce il numero di ocorrenze di c
 
 \param a[] array di struct di tipo motion da ordinare
 \param c parametro di aggiornamento
 
 \return numero di occorrenze di c nell'array
 */

int partialsort(motion a[], char c);


/** questa procedura aggiorna i puntatori k,l con le nuove coordinate dell'elemento
 \param (x,y) coordinate iniziali dell'elemento
 \param (k,l) puntatori da aggiornare con le nuove coordinate
 \param random indice calcolato casualmente utilizzato per accedere all'array motion per estrarne le nuove coordinate
 \param move array di struct di tipo motion che contiene le informazione necessarie a calcolare le nuove coordinate
 */

void new_coordinate(int x, int y, int *k, int *l, int random, motion move[]);


#endif
