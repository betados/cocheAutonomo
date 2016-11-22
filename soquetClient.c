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

char * append(char * string1, char * string2)
{
    char * result = NULL;
    asprintf(&result, "%s%s", string1, string2);
    return result;
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[1024];
	//char nuevo[256];
	char * nuevo ;


    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
do{
	//mensaje espurio para reiniciar parseInt() en arduino
	strcpy(buffer, "espurio ");
	n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR escribiendo el mensaje espureo");

    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
	// append	
	nuevo=append("cadena ",buffer);
	printf("%s\n", nuevo);
	//strcat(nuevo,buffer);
	printf("%s\n", nuevo);
    n = write(sockfd,nuevo,strlen(nuevo));
    if (n < 0) 
         error("ERROR writing to socket");
    
}while(1);
    close(sockfd);
    return 0;
}


