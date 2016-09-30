#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>

#define BUFF_SIZE 512*1024 // 512KB = 524228 bytes
//#define DEBUG

#define GET_TIME(a) clock_gettime(CLOCK_REALTIME, a)

void main(int argc, char *argv[]){
    int data_size;
    long iter, i;
    char buffer[BUFF_SIZE];
    int wt_size, rd_size;
    long rd_data_size;

    struct timespec s_time, e_time;
    int sec_dif;
    long nsec_dif;
    
    double throughput = 0;

    int pipe_fd[2]; // 0 : read / 1 : write
    int ack_pipe_fd[2];

    if(argc != 2){
	printf("Usage : pipe [number_of_bytes]\n");
	exit(0);
    }

    data_size = strtol(argv[1],NULL,10);

    if(pipe(pipe_fd) == -1){
	perror("Error on pipe()");
	exit(1);
    }

    if(pipe(ack_pipe_fd) == -1){
	perror("Error on pipe()");
	exit(1);
    }

    memset(buffer, 'a', sizeof(buffer));
    iter = 1024*1024*1024 / data_size;

    switch(fork()){
	case -1 :
	perror("Error on fork()");
	exit(1);

	case 0 : // child process (write)
	close(pipe_fd[0]);
	close(ack_pipe_fd[1]);

	GET_TIME(&s_time);
	for(i=0;i<iter;i++){
	    wt_size = write(pipe_fd[1], buffer, data_size);
	}

	// raed ack
	rd_size = read(ack_pipe_fd[0], buffer, 1);
	GET_TIME(&e_time);
	
	sec_dif = e_time.tv_sec - s_time.tv_sec;
	nsec_dif = e_time.tv_nsec - s_time.tv_nsec;

	throughput = (double)1024/( (double)sec_dif + (nsec_dif * 1e-9) ); // MBps

	printf("\n1GB with %d bytes, %d sec, %ld nsec / throughput = %f MB/s\n", data_size, (int)sec_dif, nsec_dif, throughput);
	
	exit(0);

	default : // parent process (read)
	close(pipe_fd[1]);
	close(ack_pipe_fd[0]);

	rd_data_size = 0;
	
	while(rd_data_size != 1024*1024*1024){
	    rd_size = read(pipe_fd[0], buffer, data_size);
	    rd_data_size += rd_size;
//	    printf("received %ld\r", rd_data_size);
	}
	    
	// send ack
	write(ack_pipe_fd[1], buffer, 1);

	exit(0);
    }
}
