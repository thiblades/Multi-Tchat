
#include "server.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

int main(int argc, const char * argv[]) {
	InitClients();
	
	struct sockaddr_in serverAddr;
	RemplirAdresse(&serverAddr, SERVER_IP, SERVER_PORT);
	
	int server = socket(PF_INET, SOCK_STREAM, 0);
	CHECK(server);
	
	EmpecherErreurAdressAlreadyInUse(server);
	
	CHECK(bind(server, (struct sockaddr*) &serverAddr, sizeof(serverAddr)));
	CHECK(listen(server, BACKLOG));
	
	fprintf(stderr, "Listening on %s:%d\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));
	
	while(1){
		// On accepte un nouveau client.
		struct sockaddr_in clientAddr;
		socklen_t clientAddrLen = sizeof(clientAddr);
		
		int clientSock = accept(server, (struct sockaddr*) &clientAddr, &clientAddrLen);
		
		if( clientSock == -1 && errno == EINTR ){
			errno = 0;
			continue;
		}
		
		CHECK(clientSock);
		
		fprintf(stderr, "accept: client on %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
		
		// On crée la structure client
		Client client;
		memset(&client, 0, sizeof(client));
		client.socket = clientSock;
		client.addr = clientAddr;
		
		// Et on l'enregistre. Le module client s'occupe de un thread pour
		// s'occuper du client etc.
		EnregistrerClient(client);
		
		// NOTE: On ne close() pas le socket client. Ce socket appartient
		//       maintenant au module client, c'est donc à lui de le fermer
		//       quand ce sera nécessaire.
	}
	
	return 0;
}
