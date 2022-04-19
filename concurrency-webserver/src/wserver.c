#include <stdio.h>
#include "io_helper.h"
#include "threadpool.h2"
#include "queue.h"

char default_root[] = ".";
extern queue_t jobs;

//
// ./wserver [-d <basedir>] [-p <portnum>] 
// 
int main(int argc, char *argv[]) {
    int c;
    char *root_dir = default_root;
    int port = 10000;
    int numThreads = 1;
    int numBuffers = 0;
    char schedalg[50];

    while ((c = getopt(argc, argv, "d:p:t:b:s:")) != -1){
        switch (c) {
            case 'd':
                root_dir = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 't':
                numThreads = atoi(optarg);
                break;
            case 'b':
                numBuffers = atoi(optarg);
                break;
            case 's':
                strcpy(schedalg, optarg);
                break;
            default:
                fprintf(stderr, "usage: wserver [-d basedir] [-p port]\n");
                exit(1);
        }
    }

    printf("Root Directory: %s\n", root_dir);
    printf("Port Number: %d\n", port);
    printf("Number of Threads: %d\n", numThreads);
    printf("Number of Buffers: %d\n", numBuffers);
    printf("Schedalg: %s\n", schedalg);


    thread_pool_init(numThreads);
    // run out of this directory
    chdir_or_die(root_dir);

    // now, get to work
    int listen_fd = open_listen_fd_or_die(port);
    while (1) {
        struct sockaddr_in client_addr;
        int client_len = sizeof(client_addr);
        int conn_fd = accept_or_die(listen_fd, (sockaddr_t *) &client_addr, (socklen_t *) &client_len);
//        Queue_Enqueue(&jobs, conn_fd);
    }
    return 0;
}


    


 
