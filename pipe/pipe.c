#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>

#define BUFF_SIZE 512*1024 // 512KB = 524228 bytes

#define GET_TIME(a) clock_gettime(CLOCK_REALTIME, a)

void main(int argc, char *argv[]){
    int data_size, iter, i;
    char buffer[BUFF_SIZE];
    int wt_size, rd_size;

    struct timespec time;
    unsigned long min_dif = -1;
    
    double throughput = 0;

    int pipe_fd[2]; // 0 : read / 1 : write

    if(argc != 3){
	printf("Usage : pipe [number_of_bytes] [number_of_iteration]\n");
	exit(0);
    }

    data_size = strtol(argv[1],NULL,10);
    iter = strtol(argv[2],NULL,10);

    if(pipe(pipe_fd) == -1){
	perror("Error on pipe()");
	exit(1);
    }

    memset(buffer, 'a', sizeof(buffer));

    switch(fork()){
	case -1 :
	perror("Error on fork()");
	exit(1);

	case 0 : // child process (write)
	close(pipe_fd[0]);
	for(i=0;i<iter;i++){
	    GET_TIME(&time);
	    wt_size = write(pipe_fd[1], buffer, data_size);
	    printf("[%d]child  : sent %d\t, %ld\t sec, %ld\t nsec\n", i, wt_size, time.tv_sec, time.tv_nsec);
	    // send time struct
	    memcpy(buffer, &time, sizeof(time));
	    write(pipe_fd[1], buffer, sizeof(time));
	    // wait for ready.
	    usleep(1000);
	}
	
	exit(0);

	default : // parent process (read)
	close(pipe_fd[1]);
	for(i=0;i<iter;i++){
	    struct timespec recv_time;
	    long nsec_dif;

	    rd_size = read(pipe_fd[0], buffer, data_size);
	    GET_TIME(&time);
	    printf("[%d]parent : recv %d\t, %ld\t sec, %ld\t nsec\n", i, rd_size, time.tv_sec, time.tv_nsec);
	    
	    // recv time struct
	    read(pipe_fd[0], buffer, sizeof(recv_time));
	    memcpy(&recv_time, buffer, sizeof(recv_time));

	    nsec_dif = time.tv_nsec - recv_time.tv_nsec;
	    if(min_dif>nsec_dif) min_dif = nsec_dif;
	}
	
	throughput = ((double)data_size/min_dif)*1e6; // ms

	printf("%d tried, minimum time : %ld nanoseconds, throughput : %f bytes/ms\n",iter,min_dif,throughput);

	exit(0);
    }
}
