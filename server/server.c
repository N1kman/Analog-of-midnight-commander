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
#include <stdbool.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>

#define BUF_SIZE 1024
#define PORT 8181
#define IP "127.0.0.1"
#define FILEPATH "./resources/"

struct sockaddr_in client_addr;
socklen_t addrlen;
int sockfd_for_listen;
int sockfd_for_connect;
pthread_t tid;
int flag = 1;

//получение сокета
int _socket(int domain, int type, int protocol) {
     return socket(domain, type, protocol);
}

//связывание сокета
int _bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    return bind(sockfd, addr, addrlen);
}

//прослушивание
int _listen(int sockfd, int backlog) {
    return listen(sockfd, backlog);
}

//соединение сервера с клиентом
int _accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    return accept(sockfd, addr, addrlen); 
}

//закрытие сервера
int _close(int* fd)
{
	return close(*fd);
}

//отправка файла
int send_file(int* sock, const char* file_name)
{
    FILE* file;
    file = fopen(file_name, "rb");

    if(file != NULL)
    {
    	//ответ корректности
		send(*sock, "1", 1, MSG_WAITALL);
		
		//получение ответа
    	char get[2];
    	recv(*sock, get, 1, MSG_WAITALL);
    	if(get[0] == '0'){
       		 return -1;
    	}

		char buffer[BUF_SIZE];
		int bytes_read;
	
		//чтение и запись файла
		while ((bytes_read = fread(buffer, sizeof(char), BUF_SIZE, file)) > 0) {
    		send(*sock, buffer, bytes_read, 0);
		}

    }
    else {
    	//ответ ошибки
    	send(*sock, "0", 1, MSG_WAITALL);
    	return -1;
    }
    fclose(file);
    return 0;
}

//взятие имен
int takefilenames(char* list){
 	struct dirent* entry;  //указатель на структуру файла
    DIR* dir = NULL;       //указатель на структуру каталога

    if ((dir = opendir(FILEPATH)) == NULL) {
        return -1;
    }

    //чтение каталога
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0
                && strcmp(entry->d_name, "..") != 0) {
            //запись имен файлов
            char full_path[1024];
            snprintf(full_path, 1024, "%s/%s", FILEPATH, entry->d_name);
            struct stat st;
            stat(full_path, &st);
            if(S_ISREG(st.st_mode)){
				strncat(list, entry->d_name, 255);
				strncat(list, "\n", 3);
			}
        }
    }

    closedir(dir);
    return 0;
}

//поток для передачи файлов
void* connecting(void* ind){
	while(flag == 1){
		if(_listen(sockfd_for_listen, 1) == -1){
    		fprintf(stderr, "Listen with error\n");
		}
    	sockfd_for_connect = _accept(sockfd_for_listen, (struct sockaddr *) &client_addr, &addrlen);
    	if(sockfd_for_connect == -1){
    		fprintf(stderr, "Accept with error\n");
    	}
    	
    	char choice[2];
    	recv(sockfd_for_connect, choice, 1, MSG_WAITALL);
    	
    	if(choice[0] == '0'){
    		char list[8192] = "";
    		if(takefilenames(list) == -1){
    			fprintf(stderr, "Error with opendir\n");
    		}
    		send(sockfd_for_connect, list, 8192, MSG_WAITALL);
    	} else {
    		char file_to_get[255];
    		recv(sockfd_for_connect, file_to_get, 255, MSG_WAITALL);
    		char file[300] = FILEPATH;
    		strncat(file, file_to_get, 256);
    
    		if(send_file(&sockfd_for_connect, file) == -1){
    			fprintf(stderr, "send_file with error\n");
    		}
    	}
    	
    	if(_close(&sockfd_for_connect) == -1){
    		fprintf(stderr, "Close with error\n");
    	}
	}
	pthread_exit(NULL);
}

//старт
int main(){ 
	fprintf(stdout, "q - off server\n");
    //указание параметров подключения
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = INADDR_ANY;
    client_addr.sin_port = htons(PORT);
    addrlen = sizeof(client_addr);
    //получение сокета для прослушивания
    sockfd_for_listen = _socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd_for_connect == -1){
    	fprintf(stderr, "Socket is not create\n");
    	return -1;
    }
    //связывание сокета
    if(_bind(sockfd_for_listen, (struct sockaddr*) &client_addr, sizeof client_addr) == -1){
    	fprintf(stderr, "Waiting for resources\n");
    	return -1;
    }
    
    //создание потока
    int status = pthread_create(&tid, NULL, connecting, 0);
    
    if(status != 0){
		fprintf(stderr, "Pthread cannot created\n");
    	return -1;	
	} 
    
    char symbols[2];
	
	while(1){
    	scanf("%s", symbols);
    	if(symbols[0] == 'q'){
    		flag = 0;
    		break;
    	}
    }
    
    //закрытие сокета
    if(_close(&sockfd_for_listen) == -1){
    	fprintf(stderr, "Close with error\n");
    	return -1;
    }
}

