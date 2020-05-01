#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXLINE 8192

struct turtle_info {
  char name[100];
  int row;
  int column;
};

int ok_name(char* nm) {
  if (nm[0] >= 'a' && nm[0] <= 'z') {
    nm[0] -= 'a' - 'A';
  }
  return (nm[0] >= 'A' && nm[0] <= 'Z');
}

void get_turtle_info(turtle_info *ti) {

  // Get the turtle's name.
  do {
    printf("What is the name of your turtle? ");
    scanf("%s",ti->name);
    if (!ok_name(ti->name)) {
      printf("The turtle's name needs to start with a letter.\n");
    }
  } while (!ok_name(ti->name));

  // Get the turtle's initial position.
  printf("Please give '%s' row and column for it to start:\n",ti->name);
  printf("Row? ");
  scanf("%d",&(ti->row));
  int column;
  printf("Column? ");
  scanf("%d",&(ti->column));
  
}

void grid_session(int clientfd, char* host) {
  
  turtle_info ti;
  get_turtle_info(&ti);
  
  //
  // Read a line of input, send it, output the response.
  //
  int line = 0;
  while (1) {
    
    char buffer[MAXLINE];
    if (line == 0) {

      // Send our turtle info before beginning.
      char *s = fgets(buffer, MAXLINE, stdin); // Seem to need this to clear STDIN.
      sprintf(buffer,"%s %d %d\n",ti.name,ti.row,ti.column);

    } else {

      printf("%s %d: ", ti.name, line);
      // Grab a turtle command from the user.
      char *s = fgets(buffer, MAXLINE, stdin);

      if (s == NULL || strcmp(buffer,"quit\n") == 0) {
	// We encountered END OF FILE.  User must want to quit.
	printf("Ok. Sent %d commands. Exiting.\n", line-1);
	break;
      }

    }

    // Send it to the server.
    printf("%d: %s <-- %s", line, host, buffer);
    write(clientfd,buffer,strlen(buffer)+1);

    // Read the server's response.
    int n = read(clientfd, buffer, MAXLINE);
    if (n == 0) {
      // No bytes received.  The server closed the connection.
      printf("Server closed the connection after handling %d lines.  Exiting.\n", line-1);
      break;
    }
    printf("%d: %s --> \n",line,host);

    // Output the response to the user.
    fputs(buffer, stdout);

    line++;
  }

  //
  // Close the connection.
  //
  close(clientfd); 
}


int main(int argc, char **argv) 
{
  //
  // Check the arguments for the host name and port number of 
  // an echo service.
  //
  if (argc != 3) {
    fprintf(stderr,"usage: %s <host> <port>\n", argv[0]);
    exit(0);
  }
  
  //
  // Look up the host's name to get its IP address.
  //
  char *host = argv[1];
  int port = atoi(argv[2]);
  struct hostent *hp;
  if ((hp = gethostbyname(host)) == NULL) {
    fprintf(stderr,"GETHOSTBYNAME failed.\n");
    exit(-1);
  }

  //
  // Request a socket and get its file descriptor.
  //
  int clientfd;
  if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr,"SOCKET creation failed.\n");
    exit(-1);
  }
    
  //
  // Fill in the host/port info into a (struct sockaddr_in).
  //
  struct sockaddr_in serveraddr;
  bzero((char *) &serveraddr, sizeof(struct sockaddr_in));
  serveraddr.sin_family = AF_INET;
  bcopy((char *) hp->h_addr_list[0], 
	(char *)&serveraddr.sin_addr.s_addr, 
	hp->h_length);
  serveraddr.sin_port = htons(port);

  //
  // Connect to the given host at the given port number.
  //
  if (connect(clientfd,
	      (struct sockaddr *)&serveraddr, 
	      sizeof(struct sockaddr_in)) < 0) {
    fprintf(stderr,"CONNECT failed.\n");
    exit(-1);
  }


  unsigned char *ip;
  ip = (unsigned char *)&serveraddr.sin_addr.s_addr;
  printf("Connected to grid at %d.%d.%d.%d...\n",
	 ip[0], ip[1], ip[2], ip[3]);

  grid_session(clientfd,host);

  exit(0);
}
