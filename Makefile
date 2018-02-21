
COMMON_OBJ = Common/common.o Common/list.o Common/protocol.o Common/sockets.o
SERVER_OBJ = Server/main.o Server/clients.o
CLIENT_OBJ = Client/main.o
OBJECTS = ${COMMON_OBJ} ${SERVER_OBJ} ${CLIENT_OBJ}

CFLAGS := ${CFLAGS} -I. -I..
LDFLAGS := -lpthread

.PHONY: all clean

all: chat_client chat_server

clean:
	rm chat_client chat_server ${OBJECTS}

chat_client: ${CLIENT_OBJ} ${COMMON_OBJ}
	${CC} -o $@ $^ ${CFLAGS} ${LDFLAGS}

chat_server: ${SERVER_OBJ} ${COMMON_OBJ}
	${CC} -o $@ $^ ${CFLAGS} ${LDFLAGS}

%.o: %.c
	${CC} -o $@ -c $^ ${CFLAGS} ${CPPFLAGS}

