
/**
 \file
 \author Marco Barbone
 Si dichiara il contenuto di questo file in ogni sua parte è interamente a cura dell'autore

 */
#include "declaration.h"

int gest_args(int argc,char * argv[]){
    int i;
    errno=0;
    if (argc<2) return 0;
    for (i=2; i<=argc-2; i=i+2) {
        if (!strncmp(argv[i], "-n",2)) {
            wator->nwork=(int)strtol(argv[i+1],NULL,10);
            flag[0]++;
        }
        if (!strncmp(argv[i], "-v",2)) {
            wator->chronon=(int)strtol(argv[i+1],NULL,10);
            flag[1]++;
        }
        if (!strncmp(argv[i], "-f",2)) {
            strcpy(printfile,argv[i+1]);
            flag[2]++;
        }
        if (errno) return 1;
    }
    for (i=0; i<3; i++) {
        if (flag[i]>1) return 1;
    }
    return 0;
}

int set_signals(){
    int check;
    sigfillset(&set);
    check=pthread_sigmask(SIG_SETMASK, &set, NULL);
    if (check) return EXIT_FAILURE;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGPIPE);
    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGTERM);
    check=pthread_sigmask(SIG_SETMASK, &set, NULL);
    if (check) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

int inizialize_socket(){
    bzero(visual_socket.sun_path, sizeof(visual_socket.sun_path));
    strcpy(visual_socket.sun_path,SOCKNAME);
    visual_socket.sun_family=AF_UNIX;
    return socket(AF_UNIX, SOCK_STREAM, 0);
}

int connect_socket(int sa){
    errno=0;
    while(connect(sa,(struct sockaddr *) &visual_socket, sizeof(visual_socket))!=0){
        if (errno==ENOENT || errno==ECONNREFUSED) {
            errno=0;
            continue;
        }
    }
    if (errno!=0 && errno!=ENOENT && errno!=ECONNREFUSED) {
        perror("fallita la connessione alla socket");
        close(sa);
        return  EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int inizialize_dir(){
    errno=0;
    unlink(SOCKNAME);
    if (errno && errno!=ENOENT) {
        perror("fallita la unlink");
    }
    errno=0;
    rmdir("./tmp");
    if (errno && errno!=ENOENT) {
        perror("fallita la unlink");
    }
    errno=0;
    mkdir("./tmp", 0755);
    ERROR("fallita la mkdir")
    chmod("./tmp", 0755);
    ERROR("fallita la chmod")
    return errno;
}
