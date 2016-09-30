#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include <fcntl.h>

//#define DEBUG_PRINT
#define ITER_COUNT 2

/*
    To check time interval for doing nothing.
*/
void test_time_interval(){
    struct timespec start, end;
    int i;
    time_t sec_dif;
    long nsec_dif;

    unsigned long nsec_total=0, nsec_min=-1, nsec_max=0;

    // to avoid error on first call.
    clock_gettime(CLOCK_REALTIME, &start);
    clock_gettime(CLOCK_REALTIME, &end);

    for(i=0;i<ITER_COUNT;i++){
	clock_gettime(CLOCK_REALTIME, &start);
	// EMPTY HERE.
	clock_gettime(CLOCK_REALTIME, &end);

	sec_dif = end.tv_sec - start.tv_sec;
	nsec_dif = end.tv_nsec - start.tv_nsec;

	nsec_total += nsec_dif;
	if(nsec_max<nsec_dif) nsec_max = nsec_dif;
	if(nsec_min>nsec_dif) nsec_min = nsec_dif;

#ifdef DEBUG_PRINT
	printf("%usec %ldnesc\n", (unsigned int)sec_dif, nsec_dif);
#endif
    }

    printf("EMPTY TIME ->\t\t\tAvg: %ld nsec, Min: %ld nsec, Max: %ld nsec\n", nsec_total/ITER_COUNT, nsec_min, nsec_max);
}

/*
    To check time interval for simple kernel call (getuid)
*/
void test_getuid_interval(){

    struct timespec start, end;
    int i;
    time_t sec_dif;
    long nsec_dif;

    unsigned long nsec_total=0, nsec_min=-1, nsec_max=0;
    
    // to avoid error on first call.
    clock_gettime(CLOCK_REALTIME, &start);
    clock_gettime(CLOCK_REALTIME, &end);

    for(i=0;i<ITER_COUNT;i++){
	clock_gettime(CLOCK_REALTIME, &start);

	// INSERT KERNEL CALL HERE //
	getuid();

	clock_gettime(CLOCK_REALTIME, &end);

	sec_dif = end.tv_sec - start.tv_sec;
	nsec_dif = end.tv_nsec - start.tv_nsec;

	nsec_total += nsec_dif;
	if(nsec_max<nsec_dif) nsec_max = nsec_dif;
	if(nsec_min>nsec_dif) nsec_min = nsec_dif;

#ifdef DEBUG_PRINT
	printf("%usec %ldnesc\n", (unsigned int)sec_dif, nsec_dif);
#endif
    }

    printf("getuid() KERNEL CALL TIME ->\tAvg: %ld nsec, Min: %ld nsec, Max: %ld nsec\n", nsec_total/ITER_COUNT, nsec_min, nsec_max);
}

/*
    To check time interval for simple kernel call (getgid)
*/
void test_getgid_interval(){
    struct timespec start, end;
    int i;
    time_t sec_dif;
    long nsec_dif;

    unsigned long nsec_total=0, nsec_min=-1, nsec_max=0;
    
    // to avoid error on first call.
    clock_gettime(CLOCK_REALTIME, &start);
    clock_gettime(CLOCK_REALTIME, &end);

    for(i=0;i<ITER_COUNT;i++){
	clock_gettime(CLOCK_REALTIME, &start);

	// INSERT KERNEL CALL HERE //
	getgid();

	clock_gettime(CLOCK_REALTIME, &end);

	sec_dif = end.tv_sec - start.tv_sec;
	nsec_dif = end.tv_nsec - start.tv_nsec;

	nsec_total += nsec_dif;
	if(nsec_max<nsec_dif) nsec_max = nsec_dif;
	if(nsec_min>nsec_dif) nsec_min = nsec_dif;

#ifdef DEBUG_PRINT
	printf("%usec %ldnesc\n", (unsigned int)sec_dif, nsec_dif);
#endif
    }

    printf("getgid() KERNEL CALL TIME ->\tAvg: %ld nsec, Min: %ld nsec, Max: %ld nsec\n", nsec_total/ITER_COUNT, nsec_min, nsec_max);
}

/*
    To check time interval for complex kernel call (malloc)
*/
void test_malloc_interval(){
    struct timespec start, end;
    int i;
    time_t sec_dif;
    long nsec_dif;

    int * an_integer;

    unsigned long nsec_total=0, nsec_min=-1, nsec_max=0;
    
    // to avoid error on first call.
    clock_gettime(CLOCK_REALTIME, &start);
    clock_gettime(CLOCK_REALTIME, &end);

    for(i=0;i<ITER_COUNT;i++){
	clock_gettime(CLOCK_REALTIME, &start);

	// INSERT KERNEL CALL HERE //
	an_integer = (int*)malloc(sizeof(int)); // 1. must be freed later.
	
	clock_gettime(CLOCK_REALTIME, &end);

	free(an_integer); // 1. freed.

	sec_dif = end.tv_sec - start.tv_sec;
	nsec_dif = end.tv_nsec - start.tv_nsec;

	nsec_total += nsec_dif;
	if(nsec_max<nsec_dif) nsec_max = nsec_dif;
	if(nsec_min>nsec_dif) nsec_min = nsec_dif;

#ifdef DEBUG_PRINT
	printf("%usec %ldnesc\n", (unsigned int)sec_dif, nsec_dif);
#endif
    }

    printf("malloc() KERNEL CALL TIME ->\tAvg: %ld nsec, Min: %ld nsec, Max: %ld nsec\n", nsec_total/ITER_COUNT, nsec_min, nsec_max);
}

/*
    To check time interval for complex kernel call (open)
*/
#define FILE_PATH "/afs/cs.wisc.edu/u/m/i/mingi/afile"
void test_openfile_interval(){
    struct timespec start, end;
    int i;
    time_t sec_dif;
    long nsec_dif;

    int fd;

    unsigned long nsec_total=0, nsec_min=-1, nsec_max=0;
    
    // to avoid error on first call.
    clock_gettime(CLOCK_REALTIME, &start);
    clock_gettime(CLOCK_REALTIME, &end);

    for(i=0;i<ITER_COUNT;i++){
	clock_gettime(CLOCK_REALTIME, &start);

	// INSERT KERNEL CALL HERE //
	fd = open(FILE_PATH,O_RDONLY);
	
	clock_gettime(CLOCK_REALTIME, &end);


	sec_dif = end.tv_sec - start.tv_sec;
	nsec_dif = end.tv_nsec - start.tv_nsec;

	nsec_total += nsec_dif;
	if(nsec_max<nsec_dif) nsec_max = nsec_dif;
	if(nsec_min>nsec_dif) nsec_min = nsec_dif;

#ifdef DEBUG_PRINT
	printf("%usec %ldnesc\n", (unsigned int)sec_dif, nsec_dif);
#endif
    }

    printf("open() KERNEL CALL TIME ->\tAvg: %ld nsec, Min: %ld nsec, Max: %ld nsec\n", nsec_total/ITER_COUNT, nsec_min, nsec_max);
}

void main(){
    printf("%d times iterated\n", ITER_COUNT);

    test_time_interval();
    test_getuid_interval();
    test_getgid_interval();
}
