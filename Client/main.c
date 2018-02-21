
#include "client.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

#define MIN(a, b) (a) < (b) ? (a) : (b)
#define MAX(a, b) (a) > (b) ? (a) : (b)

bool starts_with(const char *str, const char *with){
	return memcmp(str, with, MIN(strlen(str), strlen(with))) == 0;
}

void CollectName(int server){
	char name[NAME_MAX];
	
	// Demander le nom
	printf("Entrez votre nom: ");
	ReadLine(name, NAME_MAX);
	
	// L'envoyer au serveur
	Message req;
	req.type = REQ_CLIENT_NAME;
	strcpy(req.contenu, name);
	req.tailleContenu = (int) strlen(req.contenu);
	EnvoyerMessage(server, &req);
}

void * ServerListener(int *p){
	int server = *p;
	
	while(1){
		Message req;
		
		if( !RecevoirMessage(server, &req) )
			break;
		
		if( req.type == REQ_SERVER_MESSAGE ){
			printf("[message] %s\n", req.contenu);
		} else {
			fprintf(stderr, "Requête non reconnue: %d. Ignorée.\n", req.type);
			continue;
		}
	}
	
	pthread_exit(NULL);
	return NULL;
}

static void * ServerThread(void *arg){
	struct sockaddr_in serverAddr;
	RemplirAdresse(&serverAddr, "0.0.0.0", SERVER2_PORT);
	
	int serverSocket = socket(PF_INET, SOCK_STREAM, 0);
	CHECK(bind(serverSocket, (struct sockaddr*) &serverAddr, sizeof(serverAddr)));
	CHECK(listen(serverSocket, BACKLOG));
	
	while(1){
		struct sockaddr_in clientAddr;
		socklen_t clientAddrLen = sizeof(clientAddr);
		int clientSocket = accept(serverSocket, (struct sockaddr*) &clientAddr, &clientAddrLen);
		
		if( clientSocket == -1 )
			break;
		
		Message req;
		RecevoirMessage(clientSocket, &req);
		
		printf("%s:%d: %s\n", 
			   inet_ntoa(clientAddr.sin_addr), 
			   ntohs(clientAddr.sin_port),
			   req.contenu);
		
		close(clientSocket);
	}
	
	return NULL;
}

int main(int argc, const char * argv[]) {
	int server = 0;
	struct sockaddr_in serverAddr;
	
	// Se connecter au serveur
	RemplirAdresse(&serverAddr, SERVER_IP, SERVER_PORT);
	CHECK(server = socket(PF_INET, SOCK_STREAM, 0));
	CHECK(connect(server, (struct sockaddr*) &serverAddr, sizeof(serverAddr)));
	
	// On prépare un thread pour écouter ce que le serveur nous dit
	pthread_t serverThread, listenerThread;
	pthread_create(&serverThread, NULL, (void*(*)(void*)) ServerListener, &server);
	pthread_create(&listenerThread, NULL, (void*(*)(void*)) ServerThread, NULL);
	// Et pendant qu'il fait ça, nous, on écoute l'utilisateur
	
	// Demander le nom
	CollectName(server);
	
	// Envoi de messages
	while(1){
		char line[MESSAGE_MAX];
		ReadLine(line, MESSAGE_MAX);
		
		Message req;
		
		if( starts_with(line, "/quit") ){
			req.type = REQ_CLIENT_LEAVE;
			req.tailleContenu = 0;
			EnvoyerMessage(server, &req);
			break;
		} else if( starts_with(line, "@") ){
			char ip[255], msg[255];
			char *premier_espace = strchr(line, ' ');
			size_t ip_length = premier_espace - line - 1;
			memcpy(ip, line + 1, ip_length);
			ip[ip_length] = '\0';
			strcpy(msg, premier_espace + 1);
			
			printf("Connecting to %s...\n", ip);
			struct sockaddr_in addr;
			RemplirAdresse(&addr, ip, SERVER2_PORT);
			
			int sock;
			CHECK(sock = socket(PF_INET, SOCK_STREAM, 0));
			CHECK(connect(sock, (struct sockaddr*) &addr, sizeof(addr)));
			
			req.type = 69;
			strcpy(req.contenu, msg);
			req.tailleContenu = (int) strlen(req.contenu);
			EnvoyerMessage(sock, &req);
			CHECK(close(sock));
		} else {
			req.type = REQ_CLIENT_MESSAGE;
			req.tailleContenu = (int) strlen(line);
			strcpy(req.contenu, line);
			EnvoyerMessage(server, &req);
		}
	}
	
	pthread_cancel(listenerThread);
	pthread_join(serverThread, NULL);
	CHECK(close(server));
	return 0;
}
