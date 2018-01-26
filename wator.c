/**
 \file
 \author Marco Barbone
 Si dichiara il contenuto di questo file in ogni sua parte è interamente a cura dell'autore
 
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "wator.h"
#include <string.h>
#include <inttypes.h>

/** trasforma una cella in un carattere
 \param a cella da trasformare
 
 \retval 'W' se a contiene WATER
 \retval 'S' se a contiene SHARK
 \retval 'F' se a contiene FISH
 \retval '?' in tutti gli altri casi
 */

char cell_to_char (cell_t a){
    switch (a) {
        case WATER: return 'W';
        case FISH: return 'F';
        case SHARK: return 'S';
        default: return '?';
    }
}

/** trasforma un carattere in una cella
 \param c carattere da trasformare
 
 \retval WATER se c=='W'
 \retval SHARK se c=='S'
 \retval FISH se c=='F'
 \retval -1 in tutti gli altri casi
 */

int char_to_cell (char c){
    switch (c) {
        case 'W': return WATER;
        case 'F': return FISH;
        case 'S': return SHARK;
        default: return -1;
    }
}

/** crea una matrice vuota (tutte le celle contengono 'W') utilizzando
 la rappresentazione con un vettore di puntatori a righe
 \param nrow numero di righe
 \param ncol numero di colonne
 \retval NULL se si sono verificati errori
 \retval m puntatore al vettore di puntatori a righe
 */

cell_t ** create_matrix_cell(unsigned int nrow, unsigned int ncol){
    unsigned int i,j;
    cell_t ** t = malloc(nrow*sizeof(cell_t*));
    if (t==NULL) return NULL;
    for (i=0; i<nrow; i++) {
        t[i]= (cell_t*) malloc(ncol*sizeof(cell_t));
        if (t[i]==NULL){
            while (i>0) {
                i--;
                free(t[i]);
            }
            return NULL;
        }
        for (j=0; j<ncol; j++) {
            t[i][j]=WATER;
        }
    }
    return t;
}

/** crea una matrice vuota (tutte le celle contengono 0) utilizzando
 la rappresentazione con un vettore di puntatori a righe
 \param nrow numero di righe
 \param ncol numero di colonne
 \retval NULL se si sono verificati errori
 \retval m puntatore al vettore di puntatori a righe
 */
unsigned short int ** create_matrix_int(unsigned int nrow, unsigned int ncol){
    unsigned int i;
    unsigned short int ** t = (unsigned short int**) malloc(nrow*sizeof(unsigned short int*));
    if (t==NULL) return NULL;
    for (i=0; i<nrow; i++) {
        t[i]= (unsigned short int*) calloc(ncol, sizeof(unsigned short int));
        if (t[i]==NULL){
                while (i>0) {
                    i--;
                    free(t[i]);
                }
                return NULL;
        }
    }
    return t;
}

/** crea una matrice vuota (tutte le celle contengono 0) utilizzando
 la rappresentazione con un vettore di puntatori a righe
 \param nrow numero di righe
 \param ncol numero di colonne
 \retval NULL se si sono verificati errori
 \retval m puntatore al vettore di puntatori a righe
 */
unsigned char ** create_matrix_char(unsigned int nrow, unsigned int ncol){
    unsigned int i;
    unsigned char ** t = (unsigned char**) malloc(nrow*sizeof(unsigned char*));
    if (t==NULL) return NULL;
    for (i=0; i<nrow; i++) {
        t[i]= (unsigned char*) calloc(ncol, sizeof(unsigned char));
        if (t[i]==NULL){
            while (i>0) {
                i--;
                free(t[i]);
            }
            return NULL;
        }
    }
    return t;
}

/** crea un nuovo pianeta vuoto (tutte le celle contengono WATER) utilizzando
 la rappresentazione con un vettore di puntatori a righe
 \param nrow numero righe
 \param ncol numero colonne
 
 \retval NULL se si sono verificati problemi nell'allocazione
 \retval p puntatore alla matrice allocata altrimenti
 */

planet_t * new_planet (unsigned int nrow, unsigned int ncol){
    planet_t *p=(malloc(sizeof(planet_t)));
    if (ncol==0||nrow==0) {
        free(p);
        return NULL;
    }
    if (p==NULL) return NULL;
    p->ncol=ncol;
    p->nrow=nrow;
    p->w=create_matrix_cell(nrow, ncol);
    if (p->w==NULL) return NULL;
    p->btime=create_matrix_int(nrow, ncol);
    if (p->btime==NULL) return NULL;
    p->dtime=create_matrix_int(nrow, ncol);
    if (p->dtime==NULL) return NULL;
    p->updated=create_matrix_char(nrow, ncol);
    if (p->updated==NULL) return NULL;
    return p;
}

/** dealloca un pianeta (e tutta la matrice ...)
 \param p pianeta da deallocare
 
 */
void free_planet (planet_t* p){
    unsigned int i;
    for (i=0; i<p->nrow; i++) free(p->w[i]);
    free(p->w);
    for (i=0; i<p->nrow; i++) free(p->btime[i]);
    free(p->btime);
    for (i=0; i<p->nrow; i++) free(p->dtime[i]);
    free(p->dtime);
    for (i=0; i<p->nrow; i++) free(p->updated[i]);
    free(p->updated);
    free(p);
}

/** stampa il pianeta su file secondo il formato di fig 2 delle specifiche, es
 
 3
 5
 W F S W W
 F S W W S
 W W W W W
 
 dove 3 e' il numero di righe (seguito da newline \n)
 5 e' il numero di colonne (seguito da newline \n)
 e i caratteri W/F/S indicano il contenuto (WATER/FISH/SHARK) separati da un carattere blank (' '). Ogni riga terminata da newline \n
 
 \param f file su cui stampare il pianeta (viene sovrascritto se esiste)
 \param p puntatore al pianeta da stampare
 
 \retval 0 se tutto e' andato bene
 \retval -1 se si e' verificato un errore (in questo caso errno e' settata opportunamente)
 
 */

int print_planet (FILE* f, planet_t* p){
    unsigned int i,j;
    if (f==NULL || p==NULL || p->w==NULL) return -1;
    if(fprintf(f, "%d\n", p->nrow)<0) return -1;
    if(fprintf(f, "%d\n", p->ncol)<0) return -1;
    for (i=0; i<p->nrow; i++) {
        for (j=0; j<p->ncol; j++) {
            if(j!=p->ncol-1)
                if(fprintf(f,"%c ",cell_to_char(p->w[i][j]))<0) return -1;
            if(j==p->ncol-1)
                if(fprintf(f,"%c",cell_to_char(p->w[i][j]))<0) return -1;
        }
        if(fprintf(f,"\n")<0) return -1;
    }
    return 0;
}


/** inizializza il pianeta leggendo da file la configurazione iniziale
 
 \param f file da dove caricare il pianeta (deve essere gia' stato aperto in lettura)
 
 \retval p puntatore al nuovo pianeta (allocato dentro la funzione)
 \retval NULL se si e' verificato un errore (setta errno)
 errno = ERANGE se il file e' mal formattato
 */

planet_t* load_planet (FILE* f){
    unsigned int i,j;
    char tmp;
    char nrow[40];
    char ncol[40];
    planet_t *p;
    errno=0;
    if (f==NULL) return NULL;
    if(fgets(nrow, 40, f)==NULL) return NULL;
    if(fgets(ncol, 40, f)==NULL) return NULL;
    i=(unsigned int)strtol(nrow, NULL, 10);
    if (errno) return NULL;
    j=(unsigned int)strtol(ncol, NULL, 10);
    if (errno) return NULL;
    p = new_planet(i, j);
    for (i=0; i<p->nrow; i++) {
        for (j=0; j<p->ncol; j++) {
            tmp=fgetc(f);
            if (errno) goto gest_err;
            fseek(f, sizeof(char),SEEK_CUR);
            if (errno) goto gest_err;
            p->w[i][j]=char_to_cell(tmp);
            if ((int)p->w[i][j]==-1) {
                errno = ERANGE;
                goto gest_err;
            }
        }
    }
    return p;
gest_err:
    if (p!=NULL) free_planet(p);
    return NULL;
}



/** crea una nuova istanza della simulazione in base ai contenuti
 del file di configurazione "wator.conf"
 
 \param fileplan nome del file da cui caricare il pianeta
 
 \retval p puntatore alla nuova struttura che descrive
 i parametri di simulazione
 \retval NULL se si e' verificato un errore (setta errno)
 */
wator_t* new_wator (char* fileplan){
    FILE *f = NULL;
    FILE *conf= fopen(CONFIGURATION_FILE, "r");
    char tmp[4];
    char number[40];
    wator_t *w= malloc(sizeof(wator_t));
    int i;
    errno=0;
    f=fopen(fileplan, "r");
    if (f==NULL || w==NULL || conf==NULL || errno) goto gest_error;
    for(i=0;i<3;i++){
        if (fgets(tmp,4,conf)==NULL) goto gest_error;
        if (fgets(number,40,conf)==NULL) goto gest_error;
        if (!strcmp(tmp,"sd ")) w->sd=(int)strtol(number, NULL, 10);
        if (!strcmp(tmp,"sb ")) w->sb=(int)strtol(number, NULL, 10);
        if (!strcmp(tmp,"fb ")) w->fb=(int)strtol(number, NULL, 10);
        if (errno)  goto gest_error;
        if (strcmp(tmp,"sd ") && strcmp(tmp,"sb ") && strcmp(tmp,"fb ")) goto gest_error;
    }
    w->plan=load_planet(f);
    w->nf=fish_count(w->plan);
    w->ns=shark_count(w->plan);
    w->nwork=4;
    w->chronon=0;
    fclose(conf);
    fclose(f);
    return w;
    
gest_error:
    if(w!=NULL) free(w);
    if(conf!=NULL) fclose(conf);
    if(f!=NULL) fclose(f);
    return NULL;
}

/** libera la memoria della struttura wator (e di tutte le sottostrutture)
 \param pw puntatore struttura da deallocare
 
 */
void free_wator(wator_t* pw){
    free_planet(pw->plan);
    free(pw);
}

/** restituisce il numero di pesci nel pianeta
 \param p puntatore al pianeta
 
 \retval n (>=0) numero di pesci presenti
 \retval -1 se si e' verificato un errore (setta errno )
 */

int fish_count (planet_t* p){
    unsigned int i,j,count=0;
    for (i=0; i<p->nrow; i++) {
        for (j=0; j<p->ncol; j++) {
            if (cell_to_char(p->w[i][j])=='F') {
                count++;
            }
        }
    }
    return count;
}
/** restituisce il numero di squali nel pianeta
 \param p puntatore al pianeta
 
 \retval n (>=0) numero di squali presenti
 \retval -1 se si e' verificato un errore (setta errno )
 */
int shark_count (planet_t* p){
    unsigned int i,j;
    int count=0;
    for (i=0; i<p->nrow; i++) {
        for (j=0; j<p->ncol; j++) {
            if (cell_to_char(p->w[i][j])=='S') count++;
        }
    }
    return count;
}


/** calcola un chronon aggiornando tutti i valori della simulazione e il pianeta
 \param pw puntatore al pianeta
 
 \return 0 se tutto e' andato bene
 \return -1 se si e' verificato un errore (setta errno)
 (non thread safe deprecata)
 */
int update_wator(wator_t * pw){
    int x,y;
    int *k;
    int *l;
    /** controllo se l'allocazione è andata a buon fine */
    k=malloc(sizeof(int));
    if (k==NULL) return -1;
    l=malloc(sizeof(int));
    if (l==NULL){
        free(k);
        return -1;
    }
    /** aggiorno tutti gli elementi della matrice da in alto a sinistra ad in basso a destra */
    for (x=0; x<ROW; x++) {
        for(y=0;y<COL;y++){
            switch (cell_to_char(PLANET[x][y])) {
                case 'S':
                    /** controllo se ho già aggiornato uno squalo altrimenti vedo se è vivo se si lo aggiorno */
                    if(UPDATE[x][y]==0 && shark_rule2(pw, x, y, k, l)==ALIVE)
                        shark_rule1 (pw ,x , y, k, l);
                    break;
                case 'F':
                    /** controllo se ho già aggiornato il pesce */
                    if(UPDATE[x][y]==0){
                        fish_rule4(pw, x, y, k, l);
                        fish_rule3(pw, x, y, k, l);
                    }
            }
        }
    }
    /** resetto la martice contendente i flag di aggiornamento */
    for (x=0; x<ROW; x++)
        for(y=0;y<COL;y++)
            UPDATE[x][y]=0;
    pw->chronon++;
    free(k);
    free(l);
    return 0;
}
