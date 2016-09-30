#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <assert.h>

#define BUFF_SIZE 512*1024

int main(int argc, char *argv[]){
    int sock_server, sock_client;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    char recv_buff[BUFF_SIZE];
    long data_size, rd_data_size, wt_data_size;
    int wt_offset;
    int rd_size, wt_size;
    int i;

    char loading_str[6] = {'-', '\\', '/', '-', '\\', '/'};

    int flag;
    int client_addr_size;

    memset(recv_buff, '0', sizeof(recv_buff)); // init buffer
    memset(&server_addr, 0, sizeof(server_addr)); // init server addr struct

    sock_server = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);	// Current System IP
    server_addr.sin_port = htons(60000);

    flag = 1;
    if(setsockopt(sock_server, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int)) == -1){
        perror("Error on setsockopt()");
        exit(1);
    }
    if(setsockopt(sock_server, SOL_SOCKET, SO_REUSEADDR, (char*) &flag, sizeof(int)) == -1){
	perror("Error on setsockopt()");
	exit(1);
    }


    if(bind(sock_server, (const struct sockaddr *)&server_addr, sizeof(server_addr)) == -1){ // bind ip and socket
	perror("Error on bind()");
	exit(1);
    }

    if(listen(sock_server, 5) == -1) {	// waiting for client connection request
	perror("Error on listen()");
	exit(1);
    }

    client_addr_size = sizeof(client_addr);
    sock_client = accept(sock_server, (struct sockaddr *)&client_addr, &client_addr_size); // accept client connection

    if(sock_client == -1){
	perror("Error on accept()");
	exit(1);
    }

    rd_data_size = 0;
    i = 0;
    while(rd_data_size != 1024*1024*1024){
        assert(rd_size = read(sock_client, recv_buff, BUFF_SIZE));
	rd_data_size += rd_size;
//	printf("receiving %c, %ld\r", loading_str[i%6], rd_data_size);
	i++;
    }
    wt_size = write(sock_client, recv_buff, 1);
    printf("\nwrite ack\n");

    close(sock_client);
}
