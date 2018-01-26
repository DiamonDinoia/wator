/**
 \file
 \author Marco Barbone
 
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

/* macro per la getione dell'errore */
#define ERROR(x) if(errno && !end){perror(x); goto gest_error;}

#define SOCKNAME "./tmp/visual.sck"


/**
 main del visualizer, il visualizer accetta una sola connessione, da parte del collector, poi di volta in volta riceve comando di terminazione, matrice, stampa, e se necessario termina, atrimenti continua in perpetuo, questo processo ha la stessa maschera dei segnali del wator, ingora tutti quelli che il wator gestisce, termina solo in caso di sigkill, o di comando di terminazione da parte del wator
 */
int main(int argc, char *argv[]) {
    /* dichiarazioni ed inizializzazioni */
    int end=0;
    int address=0;
    struct sockaddr_un visual_socket;
    int sa=0,sout=0;
    unsigned int nrow=0, ncol=0, i, resto=0, len, newline=0;
    int j;
    char tmp=0,*matrix=NULL;
    long check2=0;
    FILE *dumpfile=stdout;
    cell_t temp= WATER;
   
    /* stampa del pid del visualizer */
    fprintf(stderr,"visualizer pid %u\n",getpid());
    
    /* controllo se sto stampando a terminale o su un file */
    if (argc>1 && strncmp(argv[1],"stdout", strlen(argv[1]))) {
        dumpfile=fopen(argv[1], "w");
        if (errno) {perror("fallita la fopen\n"); goto gest_error;}
        sout=1;
    }
    
    /* inizializzo la socket */
    bzero(visual_socket.sun_path, sizeof(visual_socket.sun_path));
    strcpy(visual_socket.sun_path,SOCKNAME);
    visual_socket.sun_family=AF_UNIX;
    ERROR("errore nell'inizializzazione")
    
    /* creo la socket */
    sa = socket(AF_UNIX, SOCK_STREAM, 0);
    ERROR("fallita la creazione della socket")
    bind(sa,(const struct sockaddr*) &visual_socket, sizeof(visual_socket));
    ERROR("fallita la bind");
    listen(sa, SOMAXCONN);
    ERROR("fallita la listen")
    
    /* accetto la connessione del collector */
    address=accept(sa,NULL, 0);
    ERROR("fallita la accept")
    
    /* leggo il numero di righe e di colonne */
    check2=read(address, &nrow, sizeof(nrow));
        ERROR("fallita la prima read");
        if (check2==0) goto gest_error;
    check2=read(address, &ncol, 4);
        ERROR("fallita la seconda read");
        if (check2==0) goto gest_error;
    
    /* calcolo il numero di byte che devo ricevere ogni volta */
    resto=ncol*nrow;
    resto=resto%4;
    len=nrow*ncol>>2;
    if (resto) len++;
    
    /* alloco la matrice di buffer */
    matrix=calloc(sizeof(char)*len,sizeof(char));
    while (!end) {
    
        /* se sto stampando su file eseguo la rewind ogni volta */
        if (sout) rewind(dumpfile);
        
        /* leggo il flag di terminazione */
        check2=read(address, &end,sizeof(end));
        ERROR("fallita la read del comando di terminazione");
        if (check2==0) goto gest_error;
        
        /* stampo il numero di righe e colonne */
        fprintf(dumpfile,"%u\n", nrow);
        fprintf(dumpfile,"%u\n", ncol);
        newline=0;
        
        /* ricevo la matrice in ingresso */
        check2=read(address, matrix, sizeof(char)*len);
        ERROR("fallita una read");
        if (check2==0) goto gest_error;
        
        /* decodifico e stampo la matrice */
        for (i=0; i<len; i++) {
            tmp=matrix[i];
            for (j=0; j<4; j++) {
                temp=tmp<<(j<<1);
                temp>>=6;
                temp=temp&3;
                newline++;
                newline=newline%ncol;
                if (newline) fprintf(dumpfile,"%c ",cell_to_char(temp));
                else fprintf(dumpfile,"%c\n",cell_to_char(temp));
            }
        }
        
    }
   
    /* libero disco e ram poi termino*/
    free(matrix);
    fflush(dumpfile);
    if (sout) fclose(dumpfile);
    close(address);
    close(sa);
    printf("visualizer terminato normalmente\n");
    return EXIT_SUCCESS;

/* segmento di codice per la gestione degli errori e terminazione scorretta*/
gest_error:
    if (matrix!=NULL) {
        free(matrix);
    }
    close(address);
    close(sa);
    return EXIT_FAILURE;
}
