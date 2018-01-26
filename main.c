/**
 \file
 \author Marco Barbone
 Si dichiara il contenuto di questo file in ogni sua parte è interamente a cura dell'autore

 */


#include "declaration.h"

/* main del processo wator il quale alloca le strutture dati necessarie, crea i vari thread, ne attende la terminazione e dealloca tutto */
int main(int argc, char *argv[]) {
    int mypid=getpid(),i,check;
    pthread_t *work_tid=NULL,dispatch_tid,collect_tid,handler_tid;
    
    /* stampa del pid del wator */
    printf("wator pid: %d\n",mypid);
    /* chiamo una funzione che imposta la maschera dei segnali */
    SECALL(check, set_signals(),==EXIT_FAILURE,"fallita la set_signals")
    
    /* creo il thread che gestisce i vari segnali*/
    SECALL(check,pthread_create(&handler_tid, NULL, handler, NULL),\
           !=0, "fallita creazione handler")
    /* controllo che venga passato almeno il file da caricare */
    if (argc<1) {
        fprintf(stderr, "missing planet file\n");
        return EXIT_FAILURE;
    }
    
    /* chiamo la new wator per caricare il pianeta e allocare il wator */
    SECALL(wator,new_wator(argv[1]),==NULL, "new_wator ha fallito")
    /* imposto il numero di worker e chronon ai valori di default */
    wator->nwork=NWORK_DEF;
    wator->chronon=CHRON_DEF;
    
    /* avvio una funzione che gestisce gli argomenti... esisteva la getopt... l'ho scoperto tardi... pazienza... */
    if(gest_args(argc, argv)){
        ERROR("gest_args ha fallito")
        return EXIT_FAILURE;
    }
    
    /* inizializzo la directory di lavoro */
    errno=inizialize_dir();
    if (errno) return errno;
    /* alloco l'array contente la struttura dei lavori dei vari workers */
    SECALL(works, malloc(wator->nwork*sizeof(workingset)), ==NULL, "malloc works ha fallito")
    /* alloco un array di tid per salvare i dati dei vari thread */
    SECALL(work_tid, malloc(wator->nwork*sizeof(pthread_t)), ==NULL, "malloc work_tid ha fallito")
    /* alloco un array di lock per la sincronizzazione dei vari thread */
    SECALL(share, malloc(wator->nwork*sizeof(pthread_mutex_t)), ==NULL, "malloc share ha fallito")
    for (i=0; i<wator->nwork; i++) {
        SECALL(check, pthread_mutex_init(&share[i], NULL), !=0, "fallita l'inizializzazione di una mutex")
    }
    /* alloco un array di condition variables utilizzate per eliminare le attese attive tra i thread*/
    SECALL(waiting_wid, malloc(wator->nwork*sizeof(pthread_cond_t)), ==NULL, "malloc working ha fallito")
    for (i=0; i<wator->nwork; i++) {
        SECALL(check, pthread_cond_init(&waiting_wid[i], NULL), !=0, "fallita l'inizializzazione di una cond")
    }
    /* alloco i flag di lavoro, mi indicano se un thread sta lavorando su una sezione condivisa*/
    SECALL(working, malloc(wator->nwork*sizeof(int)), ==NULL, "malloc work_tid ha fallito")
    for (i=0; i<wator->nwork; i++) {
        working[i]=0;
    }
    
    
    /* creo il dispatcher */
    SECALL(check, pthread_create(&dispatch_tid, 0,dispatcher, NULL),\
            !=0, "creazione dispatcher fallita")
    /* creo il visualizer */
    pid=fork();
    /* inizializzo il visualizer */
    if (pid==-1) {
        perror("la fork ha fallito\n");
        goto gest_error;
    }
    if (pid==0) {
        if (strncmp(printfile, "stdout",sizeof(printfile))) {
            execl("./visualizer", "visualizer",printfile, NULL);
            perror("l'execl ha fallito\n");
            goto gest_error;
        } else {
            execl("./visualizer", "visualizer", NULL);
            perror("l'execl ha fallito");
            goto gest_error;
        }
    }

    /* creo i worker */
    for (i=0; i<wator->nwork; i++) {
        SECALL(check, pthread_create(&work_tid[i], NULL, worker, NULL),\
               !=0, "fallita la creazione di un worker")
    }
    /* creo il collector */
    SECALL(check, pthread_create(&collect_tid, NULL,collector, NULL),\
           !=0, "fallita la creazione del collector")
    
    
    /* faccio la join sui worker */
    for (i=0; i<wator->nwork; i++){
        SECALL(check, pthread_join(work_tid[i], NULL), !=0, "fallita la join su un worker")
    }
    /* chiamo la join su dispatcher, collector e handler */
    SECALL(check, pthread_join(dispatch_tid, NULL),!=0,"pthread join dispatcher fallita")
    SECALL(check, pthread_join(collect_tid, NULL), !=0, "fallita la join sul collector")
    SECALL(check, pthread_join(handler_tid, NULL), !=0, "fallita la join sul handler")
    
    /* ripulisco disco e ram e termino */
    unlink(SOCKNAME);
    ERROR("fallita la unlink")
    rmdir("./tmp");
    ERROR("fallita la rmdir")
    free_wator(wator);
    free(work_tid);
    free(works);
    free(share);
    free(working);
    free(waiting_wid);
    
    printf("main terminato\n");
    return 0;

/* frammento di codice di gestione dell'errore, sostanzialmente libera tutto il possibile termina il visualizer e finisce restituendo errore */
gest_error:
    if (pid!=-1) kill(pid, SIGKILL);
    if (wator!=NULL) free_wator(wator);
    if (works!=NULL) free(works);
    if (work_tid!=NULL) free(work_tid);
    if (share!=NULL) free(share);
    if (working!=NULL) free(working);
    unlink(SOCKNAME);
    rmdir("./tmp");
    printf("uscita\n");
    return EXIT_FAILURE;
}

