#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <queue>

// T
#define K 10

using namespace std;

// Variaveis globais
pthread_mutex_t cross;
sem_t north_sem;

queue<int> north_queue;
pthread_mutex_t read_queue;

// Batman
void *batman(void *arg); 

// Trem vindo do norte
void *north_bat(void *arg) {
	pthread_mutex_lock(&read_queue);
	int n = *(int*)arg;
	pthread_mutex_unlock(&read_queue);
	printf("Adicionou BAT %d na fila\n", n+1);
	pthread_mutex_lock(&read_queue);
	north_queue.push(n+1);
	pthread_mutex_unlock(&read_queue);
	printf("BAT %d N chegou no cruzamento\n", n+1); 	
	pthread_exit(NULL);
}

//// Main /////
int main(int argc, char *argv[]) {
	int i;
	int numBat;

	// Verifica se contém entrada
	if (argc < 2) {
		printf("Não há nenhuma sequência como entrada.\n");
		return 0;
	}

	// Numero de BATs é o tamanho da string de entrada
	numBat = strlen(argv[1]);

	// Threads para cada bat
	pthread_t batid[numBat];

	// Inicia o mutex para acesso as filas de cada direção
	pthread_mutex_init(&read_queue, NULL);


	// Inicia o semaforo de cada direção
	sem_init(&north_sem, 0, 10);

	for (i = 0; i < numBat; i++) {
		if (argv[1][i] == 'n') {
			if (pthread_create(&batid[i], NULL, north_bat, &i)) {
				printf("Erro ao criar thread %d\n", i);
			}
		} 
		/**
		  else if (argv[1][i] == 'e') {
		  pthread_create(&batid[i], NULL, east_bat, i); 
		  } 
		  else if (argv[1][i] == 'w') {
		  pthread_create(&batid[i], NULL, west_bat, i); 
		  } 
		  else if (argv[1][i] == 's') {
		  pthread_create(&batid[i], NULL, south_bat, i); 
		  } 
		 **/
		else {
			printf("Direção inválida - ['n','s','e','w']\n");
		} 
	}
	for (i = 0; i < numBat; i++) {
			if (pthread_join(batid[i], NULL)) {
				printf("Erro join thread %d\n", i);
			}
	}
	
	int tam = north_queue.size();
	for (i = 0; i < tam; i++) {
		printf("(%d) BAT %d\n", i, north_queue.front());
		north_queue.pop();
	}

	return 0;
}

