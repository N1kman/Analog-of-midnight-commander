#ifndef FUNC_SOCKETS_H
#define FUNC_SOCKETS_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define BUF_SIZE 1024
#define PORT 8181

//получение сокета
int _socket(int domain, int type, int protocol);
//соединение
int _connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
//закрытие сервера
int _close(int* fd);
//получение файла
int recv_file(int* sock, const char* file_to_get);
//взятие файла с сервера
int takefromserver(const char* file_name, const char* IP);
//взятие списка файлов с сервера
int getlistfromserver(char* list, const char* IP);

#endif // FUNC_SOCKETS_H
