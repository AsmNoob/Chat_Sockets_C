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
#include <pthread.h>

#define PORT 5555

#define BACKLOG 20

// liste des sockets
int list_sockets[100];
// liste des disponibilités (1 = place disponible | 0 == place non-disponible)
int availabilities[100] = {1};
int sockets_counter = 0;

const int8_t welcome[] = "Bienvenu, sur notre serveur de chat,\nchoisissez un pseudonyme: ";

void* distribution(void* indice_socket){
	int connected = 1;
	char nickname[100];
	char buffer[2048];
	char hour[5];
	time_t date;

	printf("SOCKET: %d\n", list_sockets[*(int*)indice_socket]);

	if(send(list_sockets[*(int*)indice_socket],welcome,strlen(welcome),0) == -1){
		perror("Server: recv");
		pthread_exit(NULL);	
	}

	if(recv(list_sockets[*(int*)indice_socket],nickname,sizeof(nickname),0) <= 0){
		perror("Server: recv");
		pthread_exit(NULL);
	}

	printf("LOGIN: %s\n",nickname);


	while(connected){	
		
		if(recv(list_sockets[*(int*)indice_socket],buffer,sizeof(buffer),0) <= 0){
			perror("Server: recv");
			connected = 0;
		}

		date = time(NULL);
		char* complete = ctime(&date);
		int i = 11;
		while(i < 16){
			hour[i-11] = complete[i];
			i++;
		}

		printf("Reçu:%s.",buffer);
		int index = 0;
		while(index < sockets_counter){
			printf("%d et %d\n", index, *(int*)indice_socket);
			if(availabilities[index] == 0){
				printf("Envoi:%s.", buffer );
				// l'heure exacte
				char new_buffer[2158];

				strcat(new_buffer,"[");
				strcat(new_buffer,hour);
				strcat(new_buffer,"]:");
				strcat(new_buffer,nickname);
				strcat(new_buffer,": ");
				strcat(new_buffer,buffer);

				printf("%s\n",new_buffer );

				if(send(list_sockets[index],new_buffer,strlen(new_buffer)-1,0) == -1){
					perror("Server: recv");
	    			connected = 0;	
				}
				memset(new_buffer, 0,strlen(new_buffer));
			}
			index++;
		}
	}
	close(list_sockets[*(int*)indice_socket]);
	pthread_exit(NULL);	
}

int main(int argc, char const *argv[]){
	// Initialisation des variables nécéssairesse
	//fd_set readfds;
	int new_fd, socket_s, yes=1;
	// server address information
	struct sockaddr_in my_addr;    
    // client address information
  	struct sockaddr_in their_addr;

  	pthread_t list_thread[100];

  	//unsigned int sin_size = sizeof(struct sockaddr_in);

	// Initialisation du 1er socket. S'occupe des accept()
	if ((socket_s = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		perror("server: socket 1");
		return EXIT_FAILURE;
	}

	if (setsockopt(socket_s,SOL_SOCKET,SO_REUSEADDR, &yes,sizeof(int)) == -1) {
		perror("server: setsockopt 1");
		return EXIT_FAILURE;
	}

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(PORT);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	memset(&(my_addr.sin_zero), '\0', 8);

	if (bind(socket_s, (struct sockaddr *)&my_addr,sizeof(struct sockaddr)) == -1) {
		perror("server: bind 1");
		return EXIT_FAILURE;
	}

	if (listen(socket_s, BACKLOG) == -1) {
		perror("server: listen 1");
		return EXIT_FAILURE;
	}
	
	while(1){
		printf("//----------------LOOOP-----------------//\n");
		pthread_t thread;

		//int *sock;
		//sock = malloc(1);
		int length = sizeof(struct sockaddr_in);

		if((new_fd = accept(socket_s,(struct sockaddr *)&their_addr,&length)) == -1){
			perror("server: accept");
		}
		//*sock = new_fd;
		printf("Nouvelle connection sur le server\n");
		printf("new_fd: %d\n",new_fd );
		int availability_counter = 0;
		int find_spot = 0;
		int indice_socket;
		// On va regarder si un spot a été libéré
		while(availability_counter < sockets_counter && find_spot == 0){
			printf("ON RENTRE avec counter: %d\n", sockets_counter);
			if(availabilities[availability_counter] == 1){
				list_sockets[availability_counter] = new_fd;
				availabilities[availability_counter] = 0;
				indice_socket = availability_counter;
				find_spot = 1;
			}
			availability_counter++;
		}
		// no available
		if(find_spot == 0 && sockets_counter < 100){
			printf("ON RENTRE DANS LE BON, couter: %d\n",sockets_counter);
			list_sockets[sockets_counter] = new_fd;
			availabilities[sockets_counter] = 0;
			indice_socket = sockets_counter;
			sockets_counter++;
		}

		printf("avail[%d]: %d\n",sockets_counter-1, availabilities[sockets_counter-1] );
		printf("sockets_counter[%d]: %d\n",sockets_counter-1, list_sockets[sockets_counter-1]);

		list_thread[indice_socket] = thread;


		if(pthread_create(&thread,NULL,distribution,(void *) &indice_socket ) != 0){
			perror("Server: pthread_create(sending)");
    		return EXIT_FAILURE;
		}



		// pid = fork();
		// if(pid == 0){
		// 	printf("//----------Processus Fils ----------//\n");
		// 	close(socket_s);
		// 	int sock = new_fd;
		// 	int connected = 1;
		// 	int indice = 0;
		// 	while( indice < sockets_counter){
		// 		printf("list[%d]: %d\n",indice,list_sockets[indice] );
		// 		indice++;
		// 	}

		// 	while(connected){
		// 		char buffer[4096];
		// 		printf(" fd:%d\n", new_fd);
		// 		if(recv(sock,buffer,sizeof(buffer),0) == -1){
		// 			perror("Server: recv");
	 //    			return EXIT_FAILURE;
		// 		}
		// 		int index = 0;
		// 		while(index < sockets_counter){
		// 			printf("ON RENTRE\n");
		// 			if(availabilities[index] == 0){
		// 				if(send(list_sockets[index],buffer,sizeof(buffer),0) == -1){
		// 					perror("Server: recv");
		// 	    			return EXIT_FAILURE;
		// 				}
		// 			}
		// 			index++;
		// 		}
		// 	}
		// }

	  	// list_sockets[0] = socket_s;
	  	// list_sockets[1] = sockfd2;
	  	// list_sockets[2] = sockfd3;

		// Initialisation des fd_set
		// FD_ZERO(&readfds);
		// int counter = 0;
		// while(counter < sockets_counter){
		// 	FD_SET(list_sockets[counter], &readfds);
		// 	counter++;
		// }
		// calcul de l'identifiant de socket e plus grand (pour savoir sur quoi il doit écouter)

  		//max = find_greater(list_sockets,sockets_counter); // max_des _fd+1 

		 //int  select( int  n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,  struct  timeval *timeout);
  // 		printf("AVANT SELECT\n");
		// if(select(max+1, &readfds, NULL, NULL, NULL) == -1){ // Lancement de select et comme prévu le writefds,exceptfds,et timout sont à NULL (donc par défaut)
		// 	perror("server: select");
		// 	return EXIT_FAILURE;
		// }else{
		// 	printf("SELECT WENT WELL\n");
		// 	if(FD_ISSET(list_sockets[0],&readfds)){
		// 		printf("ACCEPTED\n");
		// 		if(new_fd = accept(socket_s,(struct sockaddr *)&their_addr,&sin_size) == -1){
		// 			perror("server: accept");
		// 		}
		// 		printf("new_fd: %d\n",new_fd );
		// 		sockets_counter+=1;
		// 		list_sockets[sockets_counter-1] = new_fd;
		// 		printf("sockets_counter[sockets_counter]: %d\n", list_sockets[sockets_counter-1]);
		// 	}else{
		// 		counter = 0;
		// 		while(counter < sockets_counter){
		// 			if(FD_ISSET(list_sockets[counter],&readfds)){
		// 				printf("READ\n");
		// 			}
		// 			counter++;
		// 		}
		// 	}
				
		// }
		//Vu que le select va se réveiller mais on ne sait pas par qui, on test quel est lesocket qui l'a réveillé

		// if (FD_ISSET(socket_s, &readfds)){
		// 	printf("lol1\n");
		// 	if((new_fd = accept(socket_s,(struct sockaddr *)&their_addr,&sin_size) == -1){
		// 		perror("server: accept");
		// 	}

		// 	if(fork() === 0){
		// 		char buffer[4096];

		// 		if(send(new_fd,buffer,sizeof(buffer),0) == -1){
		// 	  		perror("Server: send");
		// 	    	return EXIT_FAILURE;
		// 	  	}

		// 	  	printf("envoyé: %s\n", buffer);

		// 		if(recv(new_fd,buffer,sizeof(buffer),0) == -1){
		// 			perror("Server: recv");
	 //    			return EXIT_FAILURE;
		// 		}

		// 		printf("reçu:%s\n", buffer);
		// 	}
		// 	close(socket_s);

		// }else if(FD_ISSET(sockfd2, &readfds)){
		// 	printf("lol2\n");
		// }else if (FD_ISSET(sockfd3, &writefds)){
		// 	printf("lol3\n");
		// }

		//Il suffit ensuite d'accept le bon socket et effectuer le service désiré dessus dans un fork pour laisser la boucle se finir et relancer le select.




	}
	int j = 0;
	while(j < sockets_counter){
		pthread_join(list_thread[j],NULL);
	}
	return 0;
}