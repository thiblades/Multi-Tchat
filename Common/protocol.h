#ifndef protocol_h
#define protocol_h

#include <stdbool.h>

#define PROTO_MESSAGE_MAX 4096

typedef enum {
	MSG_COLOR_RED		= 1,
	MSG_COLOR_GREEN		= 2,
	MSG_COLOR_YELLOW	= 3,
	MSG_COLOR_BLUE		= 4,
	MSG_COLOR_MAGENTA	= 5,
	MSG_COLOR_CYAN		= 6,
} MsgColor;

typedef enum {
	REQ_CLIENT_NAME, // Client: Choix nom.
	REQ_CLIENT_JOIN, // Client: pour rejoindre un salon.
	REQ_CLIENT_ROOM_LIST, // Client: Liste des salons.
	REQ_CLIENT_MESSAGE, // Client: Envoi message.
	REQ_CLIENT_LEAVE, // Client: Quitter.
	
	REQ_SERVER_MESSAGE, // Serveur: message envoyé
} ReqType;

typedef struct {
	int type;
	char contenu[PROTO_MESSAGE_MAX];
	int tailleContenu;
} Message;

bool RecevoirMessage(int socket, Message *msg);
void EnvoyerMessage(int socket, const Message *msg);

#endif /* protocol_h */
