#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <time.h>

#define BUFF_SIZE 512*1024 // 512 KB
#define GET_TIME(a) clock_gettime(CLOCK_REALTIME, a)

int main(int argc, char *argv[]){
    int sock_client;
    struct sockaddr_in server_addr;
    int flag, i, iter;

    char buffer[BUFF_SIZE];
    int data_size, rd_data_size;
    int rd_size, wt_size;

    struct timespec start, end;
    time_t sec_dif;
    long nsec_dif;
    unsigned long min_dif = -1;
    double throughput = 0;

    // check args
    if(argc != 4){
	printf("Usage : tcp_client [server_ip] [number_of_bytes_to_send] [number_of_iteration]\n");
	exit(0);
    }

    memset(buffer, 'a', BUFF_SIZE);
    memset(&server_addr, '0', sizeof(server_addr));

    iter = strtol(argv[3], NULL, 10);
    for(i=0;i<iter;i++){
	sock_client = socket(AF_INET, SOCK_STREAM, 0);
        if(sock_client == -1){
	    perror("Error on socket()");
	    exit(1);
	}

        // nagling disable
	flag = 1;
	if(setsockopt(sock_client, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int)) == -1){
	    perror("Error on setsockopt()");
	    exit(1);
	}
    
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	server_addr.sin_port = htons(60000);

	if(connect(sock_client, &server_addr, sizeof(server_addr)) == -1){
	    perror("Error on connect()");
	    exit(1);
	}

	data_size = strtol(argv[2], NULL, 10);
	write(sock_client, argv[2], strlen(argv[2])+1); // write including NULL character

	rd_data_size = 0;

        GET_TIME(&start);
	// write to sock
        wt_size = write(sock_client, buffer, data_size);

	// read from sock
	while(rd_data_size < data_size){
	    rd_size = read(sock_client, buffer, BUFF_SIZE);
	    rd_data_size += rd_size;
	}
        GET_TIME(&end);
        close(sock_client);

	printf("wt %d, rd %d   -> ", wt_size, rd_data_size);

        sec_dif = end.tv_sec - start.tv_sec;
        nsec_dif = end.tv_nsec - start.tv_nsec;

        printf("%d\t sec, %ld\tnsec\n", sec_dif, nsec_dif);

	if(min_dif>nsec_dif) min_dif = nsec_dif;
    }

    throughput = ((double)data_size/min_dif)*1e6;
    printf("%d tries, minimum time : %ld, throughput : %f bytes/ms\n", iter, min_dif, throughput);

    return 0;
}
