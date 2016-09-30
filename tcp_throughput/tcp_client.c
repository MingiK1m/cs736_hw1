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
    int flag;
    unsigned int iter, i;

    char buffer[BUFF_SIZE];
    int data_size, rd_data_size;
    int rd_size, wt_size;

    struct timespec start, end;
    time_t sec_dif;
    long nsec_dif;

    double throughput = 0;

    // check args
    if(argc != 3){
	printf("Usage : tcp_client [server_ip] [number_of_bytes_to_send]\n");
	exit(0);
    }

    memset(buffer, 'a', BUFF_SIZE);
    memset(&server_addr, '0', sizeof(server_addr));

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

    if(connect(sock_client, (const struct sockaddr *)&server_addr, sizeof(server_addr)) == -1){
	perror("Error on connect()");
	exit(1);
    }

    data_size = strtol(argv[2], NULL, 10);
    iter = 1024*1024*1024 / data_size;

    GET_TIME(&start);
    // write to sock
    for(i=0;i<iter;i++){
        wt_size = write(sock_client, buffer, data_size);
//	printf("writing %d\r", i);
    }

    // read ack
    rd_size = read(sock_client, buffer, 1);
    GET_TIME(&end);

    sec_dif = end.tv_sec - start.tv_sec;
    nsec_dif = end.tv_nsec - start.tv_nsec;

    close(sock_client);

    throughput = (double)1024/( (double)sec_dif + (nsec_dif * 1e-9) ); // MBps
    printf("\n1GB with %d bytes, %d sec, %ld nsec / throughput = %f MB/s\n", data_size, (int)sec_dif, nsec_dif, throughput);
//    throughput = ((double)data_size/min_dif)*1e6;
//    printf("%d tries, minimum time : %ld, throughput : %f bytes/ms, %f kb/ms\n(calculated by round trip time / 2)\n", iter, min_dif, throughput, throughput/1024);

    return 0;
}
