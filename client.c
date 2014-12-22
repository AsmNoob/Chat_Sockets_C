// Gérard Tio Nogueras - INFO2 - 000333083
// Info-f-201
// Projet n°2: Chat

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

//Librairies reseau - processus
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 5555

int main(int argc, char const *argv[])
{
	// Initialisation des variables

	int sockfd_c;
	struct sockaddr_in their_addr; // addresse du serveur
	char name[100];
	struct hostent *host;

	if(gethostname(name, sizeof(name))==-1) {
	    perror("gethostname: ");
	    return EXIT_FAILURE;  
	}

	if ((host=gethostbyname(name)) == NULL) { 
	    herror("gethostbyname: ");
	    return EXIT_FAILURE;
	}
	  
	printf("Host name  : %s\n", name);
	printf("IP Address : %s\n", inet_ntoa(*((struct in_addr *)host->h_addr)));

	if((sockfd_c = socket(PF_INET, SOCK_STREAM,0)) == -1){
		perror("Client: socket");
		return EXIT_FAILURE;
	}

  	their_addr.sin_family = AF_INET;  
  	their_addr.sin_port = htons(PORT);
	their_addr.sin_addr = *((struct in_addr*)host->h_addr);
	memset(&(their_addr.sin_zero), '\0', 8); 

	if(connect(sockfd_c, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1){
  		perror("Client: connect");
    	return EXIT_FAILURE;
  	}
  	char str[80];
  	scanf("%s",str);



	return 0;
}

