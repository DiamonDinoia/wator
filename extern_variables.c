/**
 \file
 \author Marco Barbone
 Si dichiara il contenuto di questo file in ogni sua parte è interamente a cura dell'autore

 */




#include "declaration.h"

/** inizializzazione delle variabili globali dichiarate nell'header */
int done=0;
int assigned=0;
int flag[3]={0};
int sig=0;
pid_t pid=-1;
int end=0;
workingset *works=NULL;
struct sockaddr_un visual_socket;
wator_t *wator=NULL;
char printfile[30] = {"teststampa"};
pthread_mutex_t job = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t idle = PTHREAD_COND_INITIALIZER;
sigset_t set;
pthread_mutex_t *share=NULL;
pthread_cond_t *waiting_wid=NULL;
int *working = NULL;
int dump_flag=0;
pthread_mutex_t set_dump_flag = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t wait_dump = PTHREAD_COND_INITIALIZER;
