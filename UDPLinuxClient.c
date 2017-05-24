#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int socketFD, nBytes;
    short portNum;
    struct sockaddr_in server_addr;
    struct hostent *server;
    char buffer[256];
    
    if (argc < 3) {
       fprintf(stderr,"\nUsage: %s <host ip address> <port number>\n", argv[0]);
       exit(0);
    }

    portNum = atoi(argv[2]);
    socketFD = socket(AF_INET, SOCK_DGRAM, 0);

    if (socketFD < 0){
		error("\nError in opening socket");
	}
    server = gethostbyname(argv[1]);

    if (server == NULL) {
        fprintf(stderr,"\nError: Host not found\n");
        exit(0);
    }

    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&server_addr.sin_addr.s_addr,
         server->h_length);
    server_addr.sin_port = htons(portNum);

    if (connect(socketFD,(struct sockaddr *) &server_addr,sizeof(server_addr)) < 0){
		error("ERROR connecting");
	}

    printf("\nEnter message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    nBytes = write(socketFD,buffer,strlen(buffer));
    if (nBytes < 0) 
         error("\nError occured while writing to socket!");
    bzero(buffer,256);
    nBytes = read(socketFD,buffer,255);
    if (nBytes < 0) 
         error("\nError while reading from socket!");
    printf("\nServer says: %s",buffer);
    close(socketFD);
    return 0;
}
/*
Reference: http://www.linuxhowtos.org/C_C++/socket.htm
*/
