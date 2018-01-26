/**
 \file
 \author Marco Barbone
 Si dichiara il contenuto di questo file in ogni sua parte è interamente a cura dell'autore
 
 */
#include <stdlib.h>
#include <errno.h>
#include "wator.h"
#include "rules.h"

int shark_rule1 (wator_t* pw, int x, int y, int *k, int* l){
    int i=0;
    long rand=random();
    motion move[4];
/** controllo di sicurezza */
    if (pw==NULL||k==NULL||l==NULL||x<0||y<0) return -1;
/** inizializzo l'array move con le informazioni sulle possibili nuove posizioni */
    inizialize_motion(pw, x, y, move);
/** controllo se lo squalo è circondato da squali in tal caso è inutile proseguire con la funzione */
    if(move[0].val=='S' && move[1].val=='S' && move[2].val=='S' && move[3].val=='S'){
        return STOP;
    }
/** se non sono circondato da squali tramite la funzione partialsort controllo che ci sia almeno un pesce, se si lo squalo lo mangia */
    i=partialsort(move, 'F');
    if (i) {
        new_coordinate(x, y, k, l, rand%i, move);
        UPDATE[*k][*l]=1;
        PLANET[*k][*l]=PLANET[x][y];
        PLANET[x][y]=char_to_cell('W');
        BIRTH[*k][*l]=BIRTH[x][y];
        DEATH[*k][*l]=0;
        return EAT;
    }
/** se arrivo qui vuol dire che non ho trovato nemmeno un pesce, eseguo la stessa procedura contollando se c'è acqua intorno allo squalo */
    i=partialsort(move, 'W');
    if (i) {
        new_coordinate(x, y, k, l, rand%i, move);
        UPDATE[*k][*l]=1;
        PLANET[x][y]=PLANET[*k][*l];
        PLANET[*k][*l]=char_to_cell('S');
        BIRTH[*k][*l]=BIRTH[x][y];
        DEATH[*k][*l]=DEATH[x][y];
        return MOVE;
    }
    return -1;
}

/** Regola 2: gli squali si riproducono e muoiono
 \param pw puntatore alla struttura wator
 \param (x,y) coordinate dello squalo
 \param (*k,*l) coordinate dell'eventuale squalo figlio (
 
 La funzione calcola nascite e morti in base agli indicatori
 btime(x,y) e dtime(x,y).
 
 == btime : nascite ===
 Se btime(x,y) e' minore di  pw->sb viene incrementato.
 Se btime(x,y) e' uguale a pw->sb si tenta di generare un nuovo squalo.
 Si considerano i 4 vicini
 (x-1,y)
 (x,y-1) *** (x,y+1)
 (x+1,y)
 
 Se una di queste celle e' vuota lo squalo figlio viene generato e la occupa, se le celle sono tutte occupate da pesci o squali la generazione non avviene.
 In entrambi i casi btime(x,y) viene azzerato.
 
 == dtime : morte dello squalo  ===
 Se dtime(x,y) e' minore di pw->sd viene incrementato.
 Se dtime(x,y) e' uguale a pw->sd lo squalo muore e la sua posizione viene
 occupata da acqua.
 
 NOTA: la situazione del pianeta viene
 modificata dalla funzione con il nuovo stato
 
 
 \retval DIED se lo squalo e' morto
 \retval ALIVE se lo squalo e' vivo
 \retval -1 se si e' verificato un errore (setta errno)
 */

int shark_rule2 (wator_t* pw, int x, int y, int *k, int* l){
    int i=0;
    long rand=random();
    motion move[4];
/** controllo di sicurezza */
    if (pw==NULL||k==NULL||l==NULL||x<0||y<0) return -1;
/** caso in cui lo squalo muore */
    if (DEATH[x][y]==pw->sd) {
        DEATH[x][y]=0;
        PLANET[x][y]=char_to_cell('W');
        BIRTH[x][y]=0;
        return DEAD;
    }
/** caso in cui lo squalo non muore */
    if(DEATH[x][y]<pw->sd) DEATH[x][y]++;
/** caso in cui lo squalo non si riproduce */
    if (BIRTH[x][y]<pw->sb){
        BIRTH[x][y]++;
        return ALIVE;
    }
/** caso in cui lo squalo si riproduce, se c'è più di uno spazio occupato dall'acqua in modo casuale scelgo dove far nascere un figlio */
    if (BIRTH[x][y]==pw->sb) {
        BIRTH[x][y]=0;
/** aggiorno l'array di move con le possibili coordinate */
        inizialize_motion(pw, x, y, move);
/** riordino l'array e assegno all'indice i quante caselle contenenti acqua sono intorno allo squalo */
        i=partialsort(move, 'W');
/** se non c'è nemmeno una casella contentente acqua lo squalo non si riproduce */
        if(!i) return ALIVE;
/** scelgo a caso una casella tra tutte le possibili e ci inserisco lo squalo figlio */
        new_coordinate(x, y, k, l, rand%i, move);
        PLANET[*k][*l]=char_to_cell('S');
        UPDATE[*k][*l]=1;
        BIRTH[*k][*l]=0;
        DEATH[*k][*l]=0;
        return ALIVE;
    }
    return -1;
}


/** Regola 3: i pesci si spostano
 
 \param pw puntatore alla struttura di simulazione
 \param (x,y) coordinate iniziali del pesce
 \param (*k,*l) coordinate finali del pesce
 
 La funzione controlla i 4 vicini
 (x-1,y)
 (x,y-1) *** (x,y+1)
 (x+1,y)
 
 un pesce si sposta casualmente in una delle celle adiacenti (libera).
 Se ci sono piu' celle vuote la scelta e' casuale.
 Se tutte le celle adiacenti sono occupate rimaniamo fermi.
 
 NOTA: la situazione del pianeta viene
 modificata dalla funzione con il nuovo stato
 
 \retval STOP se siamo rimasti fermi
 \retval MOVE se il pesce si e' spostato
 \retval -1 se si e' verificato un errore (setta errno)
 */


 int fish_rule3 (wator_t* pw, int x, int y, int *k, int* l){
    int i=0;
    long rand=random();
    motion move[4];
/** controlli di sicurezza */
    if (pw==NULL||k==NULL||l==NULL||x<0||y<0) return -1;
/** inserisco nell'array le informazioni necessarie al calcolo delle nuove coordinate */
    inizialize_motion(pw, x, y, move);
/** verifico se almeno una casella è libera */
    i=partialsort(move,'W');
/** se nessuna casella é libera il pesce non si può muovere */
     if (!i) {
         return STOP;
     }
    else{
/** se arrivo qui vuol dire che il pesce può muoversi, se ho più possibili movimenti ne scelgo uno a caso */
        new_coordinate(x, y, k, l, rand%i, move);
        PLANET[*k][*l]=PLANET[x][y];
        BIRTH[*k][*l]=BIRTH[x][y];
        PLANET[x][y]=char_to_cell('W');
        UPDATE[*k][*l]=1;
        return MOVE;
    }
    
    return -1;
}


/** Regola 4: i pesci si riproducono
 \param pw puntatore alla struttura wator
 \param (x,y) coordinate del pesce
 \param (*k,*l) coordinate dell'eventuale pesce figlio
 
 La funzione calcola nascite in base a btime(x,y)
 
 Se btime(x,y) e' minore di  pw->sb viene incrementato.
 Se btime(x,y) e' uguale a pw->sb si tenta di generare un nuovo pesce.
 Si considerano i 4 vicini
 (x-1,y)
 (x,y-1) *** (x,y+1)
 (x+1,y)
 
 Se una di queste celle e' vuota il pesce figlio viene generato e la occupa, se le celle sono tutte occupate da pesci o squali la generazione non avviene.
 In entrambi i casi btime(x,y) viene azzerato.
 
 NOTA: la situazione del pianeta viene
 modificata dalla funzione con il nuovo stato
 
 
 \retval  0 se tutto e' andato bene
 \retval -1 se si e' verificato un errore (setta errno)
 */

int fish_rule4 (wator_t* pw, int x, int y, int *k, int* l){
    int i=0;
    long rand=random();
    motion move[4];
/** controlli di sicurezza */
    if (pw==NULL||k==NULL||l==NULL||x<0||y<0) return -1;
/** se il pesce non può riprodursi aumento il suo contatore nascita e restituisco 0 */
    if (BIRTH[x][y]<pw->fb) {
        BIRTH[x][y]++;
        return 0;
    }
/** se il pesce può riprodursi azzero il suo contatore */
    if (BIRTH[x][y]==pw->fb) {
        BIRTH[x][y]=0;
/** inserisco le informazioni necessarie nell'array */
        inizialize_motion(pw, x, y, move);
/** riordino e controllo quante possibili caselle libere ci sono */
        i=partialsort(move, 'W');
/** se non ho caselle libere il pesce non si ripdouce */
        if(!i) return 0;
/** caso in cui il pesce può riprodursi */
        new_coordinate(x, y, k, l, rand%i, move);
        PLANET[*k][*l]=PLANET[x][y];
        BIRTH[*k][*l]=0;
        UPDATE[*k][*l]=1;
        return 0;
    }
    return -1;
}

