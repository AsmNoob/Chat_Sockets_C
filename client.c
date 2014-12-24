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
//char pseudo[100];

void* sending(void* sockfd_c){
	printf("//----IN SENDING-----//\n");

	while(connected){
		char str[2048];

		fgets(str, sizeof(str), stdin);

		int i = 0;
		while(i < sizeof(str)){
			printf("str[%d]: %s\n",i,str[i]);
			i++;
		}

		char quit[] = "/quit";
        // printf("Envoi:%s.", message);
		printf("condition: %d\n", strcmp(str,quit));
		if(strcmp(str,quit) == 0){
			connected = 0;
		}else if(send(*(int*)sockfd_c,str,sizeof(str),0) == -1){
	  		perror("Client: send2");
	    	connected = 0;
		}
		//memset(message ,0,sizeof(message));
	}
	printf("SORTIE DE SEND\n");
	pthread_exit(NULL);
}

void* receiving(void* sockfd_c){
	printf("//----IN RECEIVING-----//\n");
	while(connected){
		char buffer[2048];
		if(recv(*(int*)sockfd_c,buffer,sizeof(buffer),0) == -1){
	  		perror("Client: recv");
	    	connected = 0;
	  	}
	  	printf("Recu: %s.",buffer);
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

  	// Demande de pseudo:

  	char intro[2048];

  	if(recv(sockfd_c,intro,sizeof(intro),0) == -1){
		perror("Client: recv");
		return EXIT_FAILURE;
	}

	printf("%s", intro);

	char response[100];
	fgets(response, sizeof(response), stdin);

	if(send(sockfd_c,strtok(response, "\n"),sizeof(response),0) == -1){
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

