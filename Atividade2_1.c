#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define SRAND_VALUE 1985
#define QTD_ITR 100
#define N 40
#define NUM_THREADS 4

long soma;
long local;

long respond;
long request;

void *cliente(void* c){
	long aux = (long) c;
	long j;	

    srand(SRAND_VALUE);
	for(j = 0; j < QTD_ITR; j++) {
		while(respond != aux){
            request = aux;
        }

        //seção critica
        local = soma;
        sleep(rand()%2);
        soma = local+1;

        //saindo da seção critica
		respond = 0;
	}
	pthread_exit(NULL);
}

void *servidor(void * s){
    //servidor looping infinito atualizando o respond e o request
	while(1){
		while(request == 0);
		respond = request;		
		while(respond != 0);
		request = 0;
	}

	pthread_exit(NULL);
}

int main(void){
	pthread_t t[NUM_THREADS];
	pthread_t t_server;
	int tid;
	long i;		
	
    soma = 0;
	request = 0;
	respond = 0;
	
	pthread_create(&t_server, NULL, servidor, NULL);
	for(i = 1; i <= NUM_THREADS; i++){ 
		pthread_create(&t[i-1], NULL, cliente, (void*) i);			
	}
		
	for(i = 0; i < NUM_THREADS; i++){
		pthread_join(t[i], NULL);
	}
	
	printf("%ld\n", soma);
	return 0;
}