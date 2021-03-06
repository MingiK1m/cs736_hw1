#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>

#define BUFF_SIZE 512*1024 // 512KB = 524228 bytes
#define DEBUG

#define GET_TIME(a) clock_gettime(CLOCK_REALTIME, a)

void main(int argc, char *argv[]){
    int data_size, iter, i;
    char buffer[BUFF_SIZE];
    int wt_size, rd_size;

    struct timespec s_time, e_time;
    long nsec_dif;
    unsigned long min_dif = -1;
    
    int pipe_fd[2]; // 0 : read / 1 : write
    int ret_pipe_fd[2];

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
    if(pipe(ret_pipe_fd) == -1){
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
	close(ret_pipe_fd[1]);

	for(i=0;i<iter;i++){
	    GET_TIME(&s_time);
	    wt_size = write(pipe_fd[1], buffer, data_size);
	    rd_size = read(ret_pipe_fd[0], buffer, data_size);
	    GET_TIME(&e_time);

	    nsec_dif = e_time.tv_nsec - s_time.tv_nsec;
#ifdef DEBUG
	    printf("diff = %ld, write %d, read %d\n", nsec_dif, wt_size, rd_size);
#endif

	    if(min_dif>(nsec_dif/2)) min_dif = nsec_dif/2;
	}

	printf("datasize = %d, %d tried, minimum time : %ld nanoseconds\n",data_size,iter,min_dif);
	
	exit(0);

	default : // parent process (read)
	close(pipe_fd[1]);
	close(ret_pipe_fd[0]);

	for(i=0;i<iter;i++){
	    rd_size = read(pipe_fd[0], buffer, data_size);
	    wt_size = write(ret_pipe_fd[1], buffer, data_size);
	}

	exit(0);
    }
}
