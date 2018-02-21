#ifndef common_h
#define common_h

#define NAME_MAX 64
#define MESSAGE_MAX 512

#define SERVER_IP "172.22.206.107"
#define SERVER_PORT "1234"
#define SERVER2_PORT "1235"

#define BACKLOG 20

// Standard headers
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// Network headers
#include <sys/socket.h>
#include <arpa/inet.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

// System headers
#include <unistd.h>
#include <pthread.h>

// Networking/System
#define CHECK( _status )								\
	if( (_status) < 0 ){								\
		CHECK_error(__FILE__, __LINE__, __FUNCTION__);	\
	}

void CHECK_error(const char *file, int line, const char *func);

void RemplirAdresse(struct sockaddr_in *addr, const char *ip, const char *port);
void EmpecherErreurAdressAlreadyInUse(int socket);

// Other
void ReadLine(char *dst, size_t limit);

#endif /* common_h */
