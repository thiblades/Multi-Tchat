
#include "common.h"
#include "protocol.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

/* 
 * Protocole: (chaine)
 * <TYPE>:<LONGUEUR>:<CONTENU>\0
 * 
 * TYPE: Nombre, type du message
 * LONGUEUR: Nombre, taille du contenu (en octets)
 * CONTENU: Le contenu du message, dépend du type.
 */

bool RecevoirChaine(int socket, char *s, int max){
	int recu = 0;
	
	while( recu < max ){
		// On reçoit un caractère
		char c = 0;
		ssize_t recuCetteFois = recv(socket, &c, sizeof(char), MSG_WAITALL);
		
		// Si le recv() a planté, on crash pas tout le serveur.
		if( recuCetteFois == sizeof(char) ){
			// On l'ajoute à la chaine
			*s++ = c;
			++recu;
			
			// Et si c'était un nul, on a fini cette requête.
			if( c == '\0' )
				return true;
		} else if( recuCetteFois == 0 ){
			fprintf(stderr, "[protocol] client has disconnected on socket %d\n", socket);
			return false;
		} else {
			if( errno == EINTR ){
				errno = 0;
				continue;
			}
			
			perror("recv");
			return false;
		}
	}
	
	return true;
}

bool RecevoirMessage(int socket, Message *msg){
	memset(msg, 0, sizeof(*msg));
	
	// Recevoir la chaine.
	char donnees[PROTO_MESSAGE_MAX];
	if( !RecevoirChaine(socket, donnees, PROTO_MESSAGE_MAX) )
		return false;
	
	char *curr = donnees, *end = NULL;
	
	// Lecture champ 1: type
	end = strchr(curr, ':');
	
	if( end == NULL ){
		fprintf(stderr, "[protocol] type field not found\n");
		return false;
	}
	
	*end++ = '\0';
	msg->type = atoi(curr);
	curr = end;
	
	// Lecture champ 2: taille
	end = strchr(curr, ':');
	
	if( end == NULL ){
		fprintf(stderr, "[protocol] size field not found\n");
		return false;
	}
	
	*end++ = '\0';
	msg->tailleContenu = atoi(curr);
	curr = end;
	
	// Lecture champ 3: données
	if( strlen(curr) != msg->tailleContenu ){
		fprintf(stderr, "[protocol] contents have length %lu, but message said %d\n",
				strlen(curr), msg->tailleContenu);
		return false;
	}
	
	strcpy(msg->contenu, curr);
	return true;
}

void EnvoyerMessage(int socket, const Message *msg){
	char donnees[PROTO_MESSAGE_MAX];
	int total = snprintf(donnees, PROTO_MESSAGE_MAX, "%u:%u:%s", msg->type, msg->tailleContenu, msg->contenu);
	
	ssize_t envoye = send(socket, donnees, total + 1, 0);
	CHECK(envoye);
	
	if( envoye != total + 1 ){
		fprintf(stderr, "[protocol] failed to send %d-byte message, only %zd bytes were sent\n",
				total + 1, envoye);
	}
}
