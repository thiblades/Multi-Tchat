#ifndef server_h
#define server_h

#include "Common/common.h"
#include "Common/list.h"
#include "Common/protocol.h"

typedef struct {
	ListNode			node;
	char				name[NAME_MAX];
	int					socket;
	struct sockaddr_in	addr;
	pthread_t			thread;
} Client;

void InitClients(void);
void EnregistrerClient(Client c);

#endif /* server_h */
