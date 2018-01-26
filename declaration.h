/**
 \file
 \author Marco Barbone
 Si dichiara il contenuto di questo file in ogni sua parte è interamente a cura dell'autore

 \brief header che contiene tutte le dichiarazioni utilizzate nel secondo frammento

*/


#ifndef wator_declaration_h
#define wator_declaration_h

/** ho fatto tutte le inclusioni in questo header che poi includo in tutti i file del secondo frammento, di solito non è buona
    pratica includere header dentro header, ma in questo caso ho notato che mi servivano ovunque gli stessi header tranne qualcuno,
    così per tenere gli altri file un po' più ordinati ho preferito fare in questo modo
*/

#include "wator.h"
#include <pthread.h>
#include <sys/un.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/stat.h>

/** piccola macro per la gestione degli errori */
#define ERROR(x) if(errno){perror(x); return errno;}

#define SOCKNAME "./tmp/visual.sck"

#define PLANETFILE "test1"

#define NWORK_DEF 4

#define CHRON_DEF 60

/** macro per la gestione degli errori: in ordine setta errno, controlla il valore di ritorno, se necessario fa la perror */
#define SECALL(x,y,z,k) errno=0; \
    if((x=y)z) {perror(k); goto gest_error;}

#define DUMPFILE "wator.check"

#define LCOL wator->plan->ncol
#define LROW wator->plan->nrow
#define LPLANET wator->plan->w
#define LBIRTH wator->plan->btime
#define LDEATH wator->plan->dtime
#define LUPDATE wator->plan->updated

/** struttura che contiene il lavoro di un worker */
typedef struct {
    /** prima riga su cui un worker deve lavorare  */
    unsigned int first;
    /** ultima riga su cui un worker deve lavorare */
    unsigned int last;
} workingset;

/** dichiarazione di tutte le variabili globali che utilizzo nel progetto */
extern pid_t pid;
/* contatore dei lavori completati */
extern int done;
/* segnale ricevuto */
extern int sig;
/* ultimo lavoro assegnato */
extern int assigned;
/* coda FIFO dei lavori, implementata come un array */
extern workingset *works;
/* struttura della socket*/
extern struct sockaddr_un visual_socket;
/* il cuore di tutto il programma :) */
extern wator_t *wator;
/* stringa contentente il nome del file di stampa */
extern char printfile[30];
/* lock per accedere alla coda FIFO */
extern pthread_mutex_t job;
/* variabile di attesa per prendere/rilasciare il lavoro*/
extern pthread_cond_t idle;
/* sigmask dei segnali masherati*/
extern sigset_t set;
/* variabile per la terminazione del processo */
extern int end;
/* variabile ausiliaria per settare gli argomenti passati al main tramite terminale*/
extern int flag[3];
/* array di lock per la sezione condivisa tra le sottomatrici */
extern pthread_mutex_t *share;
/* array di condition per limitare l'attesa attiva tra i workers */
extern pthread_cond_t *waiting_wid;
/* array di flag per notificare se un worker sta lavorando sulla sezione condivisa */
extern int *working;
/* flag di dump... viene settato quando si riceve una SIGUSR1 */
extern int dump_flag;
/* variabili di sincronizzazione/condition per il dump */
extern pthread_mutex_t set_dump_flag;
extern pthread_cond_t wait_dump;
/* le altre sono variabili ti sincronizzazione oppure di supporto ai thread */


/** dichiarazioni delle varie funzioni che utilizzo in questo frammento */

/**
 \param argc numero di argomenti passati al wator
 \param *argv[] array di argomenti passati al wator
 
 questa funzione controlla i parametri passati al wator.
 
 \retval 0 se tutto è andato bene
 \retval 1 se ci sono stati errori
 */
int gest_args(int argc,char * argv[]);
/**
 questa funzione setta i segnali da da gestire
 
 \retval EXIT_SUCCESS se tutto è andato bene
 \retval EXIT_FAILURE se ci sonon stati problemi
 */
int set_signals();

/**
 questa è la funzione eseguita dal thread dispatcher
 */
void *dispatcher(void *l);
/**
 questa è la funzione eseguita dai thread worker
 */
void *worker(void *unused);
/**
 questa è la funzione eseguita dal thread collector
 */
void *collector(void *l);
/**
 questa è la funzione eseguita dal thread handler, 
 questo thread mi gestsce i segnali ricevuti
 */
void *handler(void *l);

/**
 funzione per inizializzare la socket
 \retval indirizzo della socket aperta
 */

int inizialize_socket();
/**
 \param indirizzo di una socket valida
 
 funzione per connettersi ad una socket creata in precedenza
 
 \retval EXIT_SUCCESS se la connessione è riuscita
 \retval EXIT_FAILURE altrimenti
 */
int connect_socket(int sa);

/**
 funzione per preparare la directory della socket
 
 \retval 0 se è andato tutto bene
 \retval error code se ci sono stati problemi
 */
int inizialize_dir();

#endif
