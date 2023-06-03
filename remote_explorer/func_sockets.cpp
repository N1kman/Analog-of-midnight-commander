#include "func_sockets.h"

//получение сокета
int _socket(int domain, int type, int protocol){
    return socket(domain, type, protocol);
}

//соединение
int _connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen){
    return connect(sockfd, addr, addrlen);
}

//закрытие сервера
int _close(int* fd){
    return close(*fd);
}

//получение файла
int recv_file(int* sock, const char* file_to_get){
    send(*sock, "1", 1, MSG_WAITALL);
    //отправка названия файла
    send(*sock, file_to_get, 255, MSG_WAITALL);

    //получение ответа
    char get[2];
    recv(*sock, get, 1, MSG_WAITALL);
    if(get[0] == '0'){
        return -1;
    }

    FILE* file;
    file = fopen(file_to_get, "wb");

    if(file != NULL)
    {
        send(*sock, "1", 1, MSG_WAITALL);
        char buffer[BUF_SIZE];
        int bytes_received;

        while ((bytes_received = recv(*sock, buffer, BUF_SIZE, 0)) > 0) {
            fwrite(buffer, 1, bytes_received, file);
        }
    }
    else {
        send(*sock, "0", 1, MSG_WAITALL);
        return -1;
    }
    fclose(file);
    return 0;
}

//взятие файла с сервера
int takefromserver(const char* file_name, const char* IP){
    struct sockaddr_in server_addr;
    int sockfd_for_connect;
    //указание параметров подключения
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP);
    //получение сокета
    sockfd_for_connect = _socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd_for_connect == -1){
        return -1;
    }
    //подключение
    if(_connect(sockfd_for_connect, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
        return -1;
    }
    //прием файла
    if(recv_file(&sockfd_for_connect, file_name) == -1){
        return -1;
    }
    //закрытие соединения
    if(_close(&sockfd_for_connect) == -1){
        return -1;
    }

    return 0;
}

//взятие списка файлов с сервера
int getlistfromserver(char* list, const char* IP){
    struct sockaddr_in server_addr;
    int sockfd_for_connect;
    //указание параметров подключения
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP);
    //получение сокета
    sockfd_for_connect = _socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd_for_connect == -1){
        return -1;
    }
    //подключение
    if(_connect(sockfd_for_connect, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
        return -1;
    }
    send(sockfd_for_connect, "0", 1, MSG_WAITALL);
    recv(sockfd_for_connect, list, 8192, 0);
    //закрытие соединения
    if(_close(&sockfd_for_connect) == -1){
        return -1;
    }

    return 0;
}
