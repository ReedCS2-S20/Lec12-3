#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "trail.hh"

#define MAXLINE     8192
#define CONNECTIONS 128

struct client_t {
  Grid* grid;
  int id;
  int connection;
  struct sockaddr_in address;
};

void *turtle_session(void *ci) {
  client_t *client = (client_t *)ci;

  int id = client->id;
  int connfd = client->connection;
  
  //
  // Report the client that connected.
  //
  struct hostent *hostp;
  if ((hostp = gethostbyaddr((const char *)&client->address.sin_addr.s_addr, 
			     sizeof(struct in_addr), 
			     AF_INET)) == NULL) {
    fprintf(stderr, "GETHOSTBYADDR failed for client %d.",id);
  }
  
  printf("Accepted connection from client %d %s (%s)\n", 
	 id, 
	 hostp->h_name, 
	 inet_ntoa(client->address.sin_addr));
  
  //
  // Get their lines of text and echo them back.
  //
  int recvlen;
  char* buffer = (char*)malloc(MAXLINE+1);
  if ((recvlen = read(connfd, buffer, MAXLINE)) != 0) {
    printf("Server received %d bytes from #%d: %s",recvlen,id,buffer);
    char namec[20];
    int row;
    int column;
    sscanf(buffer,"%s %d %d",namec,&row,&column);
    std::string name {namec};
    std::string display = client->grid->enter(name,Position {row, column}, East);
    strcpy(buffer,display.c_str());
    printf("%s",buffer);
    write(connfd,buffer,strlen(buffer)+1);

    while ((recvlen = read(connfd, buffer, MAXLINE)) != 0) {
      printf("Server received %d bytes from #%d: %s",recvlen,id,buffer);
      std::string display = client->grid->exec(name,buffer);
      strcpy(buffer,display.c_str());
      printf("%s",buffer);
      write(connfd,buffer,strlen(buffer)+1);
    }

    client->grid->leave(name);
  } 
  //
  // Close the client connection.
  //
  close(connfd);

  free(buffer);
  free(client);
  return NULL;
}

int main(int argc, char **argv) 
{

  // 
  // Make sure we've been given a port to listen on.
  //
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(0);
  }
  
  //
  // Open a socket to listen for client connections.
  //
  int listenfd;
  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr,"SOCKET creation failed.\n");
    exit(-1);
  }
  
  //
  // Build the service's info into a (struct sockaddr_in).
  //
  int port = atoi(argv[1]);
  struct sockaddr_in serveraddr;
  bzero((char *) &serveraddr, sizeof(struct sockaddr_in));
  serveraddr.sin_family = AF_INET; 
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); 
  serveraddr.sin_port = htons((unsigned short)port); 

  //
  // Bind that socket to a port.
  //
  if (bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(struct sockaddr)) < 0) {
    fprintf(stderr,"BIND failed.\n");
    exit(-1);
  }

  //
  // Listen for client connections on that socket.
  //
  if (listen(listenfd, CONNECTIONS) < 0) {
    fprintf(stderr,"LISTEN failed.\n");
    exit(-1);
  }
  
  fprintf(stderr,"Turtle server listening on port %d...\n",port);
  
  Grid grid {18,8};
  
  int clients = 0;

  //
  // Handle client sessions.
  //
  while (1) {

    //
    // Build a client's profile to send to a handler thread.
    //
    client_t *client = (client_t *)malloc(sizeof(client_t));

    //
    // Accept a connection from a client, get a file descriptor for communicating
    // with the client.
    //
    client->grid = &grid;
    client->id = clients;
    socklen_t acceptlen = sizeof(struct sockaddr_in);
    if ((client->connection = accept(listenfd, (struct sockaddr *)&client->address, &acceptlen)) < 0) {
      fprintf(stderr,"ACCEPT failed.\n");
      exit(-1);
    }

    //
    // Create a thread to handle the client.
    //
    pthread_t tid;
    pthread_create(&tid,NULL,turtle_session,(void *)client);
  }

  close(listenfd);
  exit(0);
}
