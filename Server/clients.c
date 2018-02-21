/*
*
*\file main.c
*/
#include "server.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static Client * listeClients = NULL;
pthread_mutex_t listeClientsMutex;
/*
*\fn InitCLients()
*
*/
void InitClients(){
	CHECK(pthread_mutex_init(&listeClientsMutex, NULL));
}
/**
*\fn TraiterClient(Client *client)
*
*\brief fonction qui va permettre de traiter la requete d'un client
*
**/
static void * TraiterClient(Client *client){
	// Dans cette boucle, on reçoit tous les messages du client.
	while(1){
		Message req;
		if( !RecevoirMessage(client->socket, &req) ){
			fprintf(stderr, "[client %s:%d] disconnected\n",
					inet_ntoa(client->addr.sin_addr), ntohs(client->addr.sin_port));
			break;
		}
		
		if( req.type == REQ_CLIENT_NAME ){
			// Un client nous a donné son nom, on le stocke.
			strcpy(client->name, req.contenu);
			fprintf(stderr, "[client %s:%d] nom: %s\n",
					inet_ntoa(client->addr.sin_addr), ntohs(client->addr.sin_port),
					client->name);
		} else if( req.type == REQ_CLIENT_MESSAGE ){
			Message msg;
			msg.type = REQ_SERVER_MESSAGE;
			snprintf(msg.contenu, MESSAGE_MAX, "%s: %s", client->name, req.contenu);
			msg.tailleContenu = (int) strlen(msg.contenu);
			
			// Un client a un message, on le transmet aux autres.
			// NOTE: On bloque le mutex pour empêcher que 2 threads envoient un
			//       message au même client en même temps et entremêlent leurs
			//       données pendant l'envoi.
			pthread_mutex_lock(&listeClientsMutex);
			
			for(Client *curr=listeClients; curr != NULL; curr=(Client*)curr->node.next){
				// On ne renvoie pas le message à son envoyeur.
				if( curr->socket == client->socket )
					continue;
				
				// Mais on l'envoie à tous les autres!
				EnvoyerMessage(curr->socket, &msg);
			}
			
			pthread_mutex_unlock(&listeClientsMutex);
		} else {
			fprintf(stderr, "[client %s:%d] requete inconnue: %d",
					inet_ntoa(client->addr.sin_addr), ntohs(client->addr.sin_port),
					req.type);
			
			break;
		}
	}
	
	fprintf(stderr, "[client %s:%d] disconnecting\n",
			inet_ntoa(client->addr.sin_addr), ntohs(client->addr.sin_port));
	
	// Quand on a terminé, on retire le client de la liste, en faisant bien
	// attention à réserver le mutex!
	// 
	// NOTE: Il est important de retirer le client de la liste AVANT de fermer
	//       le socket. Tant que le client est dans la liste, un autre thread
	//       peut essayer d'envoyer un message sur son socket, donc si on ferme
	//       le socket quand le client est encore dans la liste, le send() de
	//       l'autre thread pourrait s'exécuter sur un socket déjà fermé.
	pthread_mutex_lock(&listeClientsMutex);
	listeClients = (Client*) ListRemove((ListNode*) listeClients, (ListNode*) client);
	pthread_mutex_unlock(&listeClientsMutex);
	
	// Maintenant, on peut fermer le socket et libérer la mémoire allouée pour
	// la structure client.
	close(client->socket);
	free(client);
	
	// Et enfin on ferme le thread.
	// NOTE: Le pthread_exit() n'est pas nécessaire, puisque retourner de la
	//       fonction vaut un pthread_exit(valeur_de_retour);
	//       Ref: http://man7.org/linux/man-pages/man3/pthread_exit.3.html#NOTES
	pthread_exit(NULL);
	return NULL;
}

void EnregistrerClient(Client c){
	// Pour pouvoir ajouter le client à la liste, il faut que la structure
	// vienne d'une allocation dynamique (sinon elle est détruite au retour
	// de la fonction). Donc on en crée une copie allouée dynamiquement.
	Client *client = (Client*) malloc(sizeof(*client));
	
	if( client == NULL )
		return;
	
	*client = c;
	
	// Maintenant, on ajoute le client à la liste. Comme la liste est partagée
	// entre tous les clients, on doit d'abord acquérir le mutex qui la protège.
	pthread_mutex_lock(&listeClientsMutex);
	listeClients = (Client*) ListPush((ListNode*) listeClients, (ListNode*) client);
	pthread_mutex_unlock(&listeClientsMutex);
	
	// On peut alors démarrer le thread qui va traiter ce client.
	pthread_create(&client->thread, NULL, (void*(*)(void*)) TraiterClient, client);
	
	// Et on le détache, comme ça on n'a pas besoin de l'attendre avec
	// pthread_join().
	pthread_detach(client->thread);
}
