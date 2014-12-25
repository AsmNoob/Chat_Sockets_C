// Gérard Tio Nogueras - INFO2 - 000333083
// Info-f-201
// Projet n°2: Chat

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

//Librairies reseau - processus
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 5555

int connected = 1;
char pseudo[100];

void* sending(void* sockfd_c){
	strtok(pseudo, "\n"); 
	while(connected){
		char str[2048];
		size_t length = 0;

		do{
			fflush(stdout);
			fgets(str, sizeof(str), stdin);
			length = strlen(str)-1;
		}while(length == 0);

		if(send(*(int*)sockfd_c,str,sizeof(str),0) <= 0){
	  		perror("Client: send2");
	    	connected = 0;
		}
		memset(str ,0,strlen(str));
	}
	pthread_exit(NULL);
}

void* receiving(void* sockfd_c){
	while(connected){
		char buffer[2048];
		if(recv(*(int*)sockfd_c,buffer,sizeof(buffer),0) <= 0){
	  		perror("Client: recv");
	    	connected = 0;
	  	}
	  	printf("%s\n",buffer);
	  	fflush(stdout);
	}
	pthread_exit(NULL);
}


int main(int argc, char const *argv[])
{
	// Initialisation des variables
	pthread_t receiver,sender;
	struct sockaddr_in their_addr; // addresse du serveur
	struct hostent *host;
	int sockfd_c;
	char name[100];

	if (argc != 2) {
        fprintf(stderr, "Donner le nom de la machine distante en argument.");
        return EXIT_FAILURE;
    }

	if ((host=gethostbyname(argv[1])) == NULL) { 
	    herror("gethostbyname: ");
	    return EXIT_FAILURE;
	}
	  
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

  	// Demande de pseudo:

  	char intro[2048];

  	if(recv(sockfd_c,intro,sizeof(intro),0) == -1){
		perror("Client: recv");
		return EXIT_FAILURE;
	}
	size_t length;

	do{
		printf("%s", intro);
		fgets(pseudo,sizeof(pseudo), stdin);
		length = strlen(pseudo)-1;
	}while(length == 0);

	//fflush(stdin);

	if(send(sockfd_c,pseudo,strlen(pseudo)-1,0) == -1){
		perror("Client: send1");
		return EXIT_FAILURE;
	}

	// Lancement des threads qui vont boucler avec recv et send:

  	if(pthread_create(&sender,NULL,sending,(void *) &sockfd_c) != 0){
  		perror("Client: pthread_create(sending)");
    	return EXIT_FAILURE;
  	}

  	if(pthread_create(&receiver,NULL,receiving,(void *) &sockfd_c) != 0){
  		perror("Client: pthread_create(receiving)");
    	return EXIT_FAILURE;
  	}

  	pthread_join(sender,NULL);
  	pthread_join(receiver,NULL);
  	// sending.join();
  	// receiving.join();

  	// if(send(sockfd_c,str,sizeof(str),0) == -1){
  	// 	perror("Client: send");
   //  	return EXIT_FAILURE;
  	// }

  	// char buffer[2048];

  	// if(recv(sockfd_c,buffer,sizeof(str),0) == -1){
  	// 	perror("Client: recv");
   //  	return EXIT_FAILURE;
  	// }

  	// if(recv(sockfd_c,buffer,sizeof(str),0) == -1){
  	// 	perror("Client: recv");
   //  	return EXIT_FAILURE;
  	// }


  	// printf("received: %s\n",buffer );

	return 0;
}

