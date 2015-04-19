#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <queue>
#include <vector>

// T
#define K 10
typedef enum direction { 
	NORTH, EAST, SOUTH, WEST
} direction;

using namespace std;

//* Variaveis globais

// Fila de prioridade
queue<int> north_queue;
queue<int> east_queue;
queue<int> south_queue;
queue<int> west_queue;

// Semaphores
sem_t crossing;
sem_t pqueue;

void *north_bat(void *arg);
void *east_bat(void *arg);
void *south_bat(void *arg);
void *west_bat(void *arg);

// Batman
void *batman(void *arg){
	int num_bats[4]; // Numero de bats esperando em cada direção
	int tot_bats;
	
	pthread_t batid[4]; // Vetor para cada thread
	do {
		// Adquire número de Bats no cruzamento
		sem_wait(&pqueue); // Obtem acesso exclusivo à fila de prioridades

		// Soma o numero de elementos em cada posição	
		num_bats[NORTH] = north_queue.size(); 
		num_bats[EAST] = east_queue.size(); 
		num_bats[SOUTH] = south_queue.size(); 
		num_bats[WEST] = west_queue.size(); 
		tot_bats = num_bats[NORTH] + num_bats[EAST] + num_bats[SOUTH] + num_bats[WEST]; 

		printf("Numero de bats = %d\n", tot_bats);
		sem_post(&pqueue); // Libera acesso à fila de prioridades

		// Cria as threads para cada direção
		if (num_bats[NORTH] > 0)
			pthread_create(&batid[NORTH], NULL, north_bat, NULL);
		if (num_bats[EAST] > 0)
			pthread_create(&batid[EAST], NULL, east_bat, NULL);
		if (num_bats[SOUTH] > 0)
			pthread_create(&batid[SOUTH], NULL, south_bat, NULL);
		if (num_bats[WEST] > 0)
			pthread_create(&batid[WEST], NULL, west_bat, NULL);

		if (num_bats[NORTH] > 0)
			pthread_join(batid[NORTH], NULL);
		if (num_bats[EAST] > 0)
			pthread_join(batid[EAST], NULL);
		if (num_bats[SOUTH] > 0)
			pthread_join(batid[SOUTH], NULL);
		if (num_bats[WEST] > 0)
			pthread_join(batid[WEST], NULL);
		
	} while(tot_bats);
}

// Trem vindo do norte
// Cada vez que for executado chega na plataforma, aguarda batman chamar e imprime
void *north_bat(void *arg) {
	// Obtem o primeiro da fila
	sem_wait(&pqueue); // Obtem acesso exclusivo à fila de prioridades
	int batNumber = north_queue.front();
	// Imprime que chegou no cruzamento
	printf("BAT %d N chegou no cruzamento\n", batNumber);
	// Retira da fila
	north_queue.pop();
	sem_post(&pqueue); // Libera acesso

	// Atravessa plataforma
	sem_wait(&crossing);
	// Quando for chamado imprime que saiu
	printf("BAT %d N saiu do cruzamento\n", batNumber);
	sem_post(&crossing);

	pthread_exit(NULL);
}

// Trem vindo do leste
// Cada vez que for executado chega na plataforma, aguarda batman chamar e imprime
void *east_bat(void *arg) {
	// Obtem o primeiro da fila
	sem_wait(&pqueue); // Obtem acesso exclusivo à fila de prioridades
	int batNumber = east_queue.front();
	// Imprime que chegou no cruzamento
	printf("BAT %d E chegou no cruzamento\n", batNumber);
	// Retira da fila
	east_queue.pop();
	sem_post(&pqueue); // Libera acesso

	// Atravessa plataforma
	sem_wait(&crossing);
	// Quando for chamado imprime que saiu
	printf("BAT %d E saiu do cruzamento\n", batNumber);
	sem_post(&crossing);

	pthread_exit(NULL);
}

// Trem vindo do sul
// Cada vez que for executado chega na plataforma, aguarda batman chamar e imprime
void *south_bat(void *arg) {
	// Obtem o primeiro da fila
	sem_wait(&pqueue); // Obtem acesso exclusivo à fila de prioridades
	int batNumber = south_queue.front();
	// Imprime que chegou no cruzamento
	printf("BAT %d S chegou no cruzamento\n", batNumber);
	// Retira da fila
	south_queue.pop();
	sem_post(&pqueue); // Libera acesso

	// Atravessa plataforma
	sem_wait(&crossing);
	// Quando for chamado imprime que saiu
	printf("BAT %d S saiu do cruzamento\n", batNumber);
	sem_post(&crossing);

	pthread_exit(NULL);
}

// Trem vindo do oeste
// Cada vez que for executado chega na plataforma, aguarda batman chamar e imprime
void *west_bat(void *arg) {
	// Obtem o primeiro da fila
	sem_wait(&pqueue); // Obtem acesso exclusivo à fila de prioridades
	int batNumber = west_queue.front();
	// Imprime que chegou no cruzamento
	printf("BAT %d W chegou no cruzamento\n", batNumber);
	// Retira da fila
	west_queue.pop();
	sem_post(&pqueue); // Libera acesso

	// Atravessa plataforma
	sem_wait(&crossing);
	// Quando for chamado imprime que saiu
	printf("BAT %d W saiu do cruzamento\n", batNumber);
	sem_post(&crossing);

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

	// Monta a fila para cada direção
	for (int i = 0; i < numBat; i++)
	{
		if (argv[1][i] == 'n')
		{
			//adiciona na queue
			north_queue.push(i+1);
			printf("Adicionou BAT %d na fila\n", i+1);
		}else if(argv[1][i] == 's'){
			//adiciona na queue
			south_queue.push(i+1);
			printf("Adicionou BAT %d na fila\n", i+1);

		}else if(argv[1][i] == 'w'){
			//adiciona na queue
			west_queue.push(i+1);
			printf("Adicionou BAT %d na fila\n", i+1);

		}else if(argv[1][i] == 'e'){
			//adiciona na queue
			east_queue.push(i+1);
			printf("Adicionou BAT %d na fila\n", i+1);
		}

		else {
			printf("Direção inválida - ['n','s','e','w']\n");
			return 0;
		} 
	}
	// Threads para o batman
	pthread_t batmanager;

	// Inicia o mutex para acesso ao numero total de bats
//	pthread_mutex_init(&num_bats_mutex, NULL);

	// Inicia os semaforos
	sem_init(&crossing, 0, 1); // Acesso ao cruzamento
	sem_init(&pqueue, 0, 1); // Acesso a fila de prioridades

	//inicializa as threads por queue/direcao
	pthread_create(&batmanager, NULL, batman, NULL);
	pthread_join(batmanager, NULL);

	return 0;
}

