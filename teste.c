#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

// T
#define K 100
// Variaveis globais
pthread_mutex_t cross;
sem_t north_sem;

// Batman
void *batman(void *arg) {

	// Trem vindo do norte
	void *north_bat(void *arg) {
		int n = *(int*)arg;
		printf("BAT %d N chegou no cruzamento\n", n); 	


	}

	int main(int argc, char *argv[]) {
		int i;
		// Numero de BATs é o tamanho da string de entrada
		int numBat = strlen(argv[1]);

		// Threads para cada bat
		pthread_t batid[numBat];

		// Inicia o semaforo de cada direção
		sem_init(&north_sem, 0, 10);
		for (i = 0; i < numBat); i++) {
			if (argv[1][i] == 'n') {
				pthread_create(&batid[i], NULL, north_bat, i); 
			} 
			else if (argv[1][i] == 'e') {
				pthread_create(&batid[i], NULL, east_bat, i); 
			} 
			else if (argv[1][i] == 'w') {
				pthread_create(&batid[i], NULL, west_bat, i); 
			} 
			else if (argv[1][i] == 's') {
				pthread_create(&batid[i], NULL, south_bat, i); 
			} 
			else {
				printf("Direção inválida - ['n','s','e','w']\n");
			} 
		}

		return 0;
	}
