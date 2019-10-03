/*
    Multi_threaded telescope protocol server
*/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<signal.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
extern long command(char* str);
int socket_desc;
void sighandle (int signo);
extern char buffcom[];
void *connection_handler(void *);
void *sserver(void * t)
{
    int yes = 1, err;
    struct timeval timeout = { 10, 0 };
    struct linger lng = { 1, 0 };

    int  client_sock , c , *new_sock;
    struct sockaddr_in server , client;
    if (signal (SIGPIPE, sighandle) == SIG_ERR)
        fprintf (stderr, "cant catch SIGPIPE signaln");
    if (signal (SIGTERM, sighandle) == SIG_ERR)
        fprintf (stderr, "cant catch SIGTERM signaln");
    if (signal (SIGINT, sighandle) == SIG_ERR)
        fprintf (stderr, "cant catch SIGINT signaln");
    if (signal (SIGALRM, sighandle) == SIG_ERR)
        fprintf (stderr, "cant catch SIGALRM signaln");
    if (signal (SIGHUP, sighandle) == SIG_ERR)
        fprintf (stderr, "cant catch SIGHUP signaln");
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");


    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );
//-------------------------------------------------------------------------
    memset (&(server.sin_zero), 0, 8);

    if (setsockopt (socket_desc, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int)) == -1)
    {
        perror ("setsockopt");
        exit (1);
    }
#define TIMEOUT
#ifdef TIMEOUT
    setsockopt (socket_desc, SOL_SOCKET, SO_RCVTIMEO,
                &timeout, sizeof (struct timeval));
    setsockopt (socket_desc, SOL_SOCKET, SO_SNDTIMEO,
                &timeout, sizeof (struct timeval));
#endif
    setsockopt (socket_desc, SOL_SOCKET, SO_LINGER, &lng, sizeof (struct linger));

//------------------------------------------------------------------------     */

    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind error");
        return NULL;
    }
    puts("bind done");

    //Listen
    listen(socket_desc , 15);


    puts("Listening on port 8888");
    c = sizeof(struct sockaddr_in);
    while(1)
    {
        if ((client_sock = accept(socket_desc, (struct sockaddr *)&client,
                                  (socklen_t*)&c)) <0)
        {
            continue;
        }
      //  puts("Connection accepted");

        pthread_t sniffer_thread;
        new_sock =(int *) malloc(1);
        *new_sock = client_sock;

        if( pthread_create( &sniffer_thread , NULL ,  connection_handler ,
                            (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return NULL;
        }



        puts("Handler assigned");
    }

    if (client_sock < 0)
    {
        perror("accept failed");
        return NULL;
    }

    return 0;
}


void *connection_handler(void *socket_desc)
{

    int sock = *(int*)socket_desc;
    int read_size;
    char *message , client_message[2000];
    pthread_detach (pthread_self ());
    int s=0;

    while( (read_size = recv(sock , client_message , 40 , 0))>0)
    {
        client_message[read_size]=0;
       // printf("%s  %d\r\n",client_message,read_size);

        command( client_message );
        if (strlen(buffcom)>0)
        {
            printf("\033[s\033[16;10H %s \033[K\r\n\033[u", buffcom);
            write(sock , buffcom , strlen(buffcom));
        }

    }

    if(read_size == 0)
    {
       // puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
    close(sock);
    free(socket_desc);
    pthread_exit (NULL);
    return 0;
}

void sighandle (int signo)
{
    if ((signo == SIGTERM) || (signo == SIGINT))
    {
        printf (" SIGTERM,received,closing all connections\r\n");

        close (socket_desc);
        exit (0);
    }
    if (signo == SIGHUP)
    {
    }
    if (signo == SIGALRM)
    {
    }
    if (signo == SIGPIPE)
    {
        printf (" SIGPIPE received\r\n");

    }
}

