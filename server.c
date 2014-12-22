// Gérard Tio Nogueras - INFO2 - 000333083
// Info-f-201
// Projet n°2: Chat

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 5555

#define BACKLOG 20

// // Option de socket
// int setOptionSocket(int socket, int level, int optioName,const void *optionValue, int socketLength){
// 	int state;
// 	if ((state = setsockopt(socket,level,optioName,&optionValue,socketLength)) == -1) {
// 	    printf("Serveur: setsockopt %d",socket);
// 	    return EXIT_FAILURE; // or state
// 	}
// 	return state;
// }

// // Binding du socket
// int bindSocket(int socket, struct sockaddr_in sockaddress){
// 	sockaddress.sin_family = AF_INET;       
// 	sockaddress.sin_port = htons(PORT);  
// 	sockaddress.sin_addr.s_addr = INADDR_ANY; 
// 	memset(&(sockaddress.sin_zero), '\0', 8); 
// 	int state;
// 	if ((state = bind(socket, (struct sockaddr *)&sockaddress,  sizeof(struct sockaddr))) == -1) {
// 	    printf("Serveur: bind %d",socket);
// 	    return EXIT_FAILURE; // or state
// 	}
// 	return state;
// }


// int listenSocket(int socket,int backlog){
// 	int state;
// 	if ((state = listen(socket, backlog)) == -1) {
// 	    printf("Serveur: listen %d",socket);
// 	    return EXIT_FAILURE;
//     }
//     return state;
// }

int find_greater(int * list,int size){
	int greater = 0,i = 0;
	while( i < size){
		printf("%sdn",list[i]);
		if(list[i] > greater){
			greater = list[i];
		}
		i++;
	}
	return greater;
}

int main(int argc, char const *argv[]){
	// Initialisation des variables nécéssairesse
	fd_set readfds,writefds;
	int max, sockfd1, sockfd2, sockfd3, yes=1, new_fd;
	// their address information
	struct sockaddr_in my_addr;    
    // my address information
  	struct sockaddr_in their_addr;

  	int list_sockets[3];

  	unsigned int sin_size = sizeof(struct sockaddr_in);

	// Initialisation du 1er socket. S'occupe des accept()
	if ((sockfd1 = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Serveur: socket 1");
		return EXIT_FAILURE;
	}

	if (setsockopt(sockfd1,SOL_SOCKET,SO_REUSEADDR, &yes,sizeof(int)) == -1) {
		perror("Serveur: setsockopt 1");
		return EXIT_FAILURE;
	}

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(PORT);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	memset(&(my_addr.sin_zero), '\0', 8);

	if (bind(sockfd1, (struct sockaddr *)&my_addr,  sizeof(struct sockaddr)) == -1) {
		perror("Serveur: bind 1");
		return EXIT_FAILURE;
	}

	if (listen(sockfd1, BACKLOG) == -1) {
		perror("Serveur: listen 1");
		return EXIT_FAILURE;
	}

	sin_size = sizeof(struct sockaddr_in);

	//Initialisation du 2eme socket. S'occupe des w
	if ((sockfd2 = socket(PF_INET,	SOCK_STREAM, 0)) == -1) {
		perror("Serveur: socket 2");
		return EXIT_FAILURE;
	}

	if (setsockopt(sockfd2,SOL_SOCKET,SO_REUSEADDR, &yes,sizeof(int)) == -1) {
		perror("Serveur: setsockopt 2");
		return EXIT_FAILURE;
	}
	    
	my_addr.sin_family = AF_INET;        
	my_addr.sin_port = htons(PORT);   
	my_addr.sin_addr.s_addr = INADDR_ANY;
	memset(&(my_addr.sin_zero), '\0', 8);

	if (bind(sockfd2, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
		perror("Serveur: bind 2");
		return EXIT_FAILURE;
	}

	if (listen(sockfd2, BACKLOG) == -1) {
		perror("Serveur: listen 2");
		return EXIT_FAILURE;
	}

	if ((sockfd3 = socket(PF_INET,	SOCK_STREAM, 0)) == -1) {
		perror("Serveur: socket 2");
		return EXIT_FAILURE;
	}

	if (setsockopt(sockfd3,SOL_SOCKET,SO_REUSEADDR, &yes,sizeof(int)) == -1) {
		perror("Serveur: setsockopt 3");
		return EXIT_FAILURE;
	}
	    
	my_addr.sin_family = AF_INET;        
	my_addr.sin_port = htons(PORT);   
	my_addr.sin_addr.s_addr = INADDR_ANY;
	memset(&(my_addr.sin_zero), '\0', 8);

	if (bind(sockfd3, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
		perror("Serveur: bind 3");
		return EXIT_FAILURE;
	}

	if (listen(sockfd3, BACKLOG) == -1) {
		perror("Serveur: listen 3");
		return EXIT_FAILURE;
	}


	while(1){
		
		// Initialisation des fd_set
		// FD_ZERO(readfds&);
		// FD_ZERO(writefds&);
		FD_SET(sockfd1, &readfds);
		FD_SET(sockfd2, &readfds);
		FD_SET(sockfd3, &writefds);

		// calcul de l'identifiant de socket e plus grand (pour savoir sur quoi il doit écouter)

  		max = find_greater(list_sockets,3); // max_des _fd+1 

  		printf("%d\n",max);

		 //int  select( int  n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,  struct  timeval *timeout);

		select(max+1, &readfds, &writefds, NULL, NULL); // Lancement de select et comme prévu le writefds,exceptfds,et timout sont à NULL (donc par défaut)

		//Vu que le select va se réveiller mai 	 		s on ne sait pas par qui, on test quel est lesocket qui l'a réveillé

		if (FD_ISSET(sockfd1, &readfds)){
			new_fd = accept(sockfd1,(struct sockaddr *)&their_addr,&sin_size);
		}

		else if(FD_ISSET(sockfd2, &readfds)){}

		//Il suffit ensuite d'accept le bon socket et effectuer le service désiré dessus dans un fork pour laisser la boucle se finir et relancer le select.




	}
	return 0;
}