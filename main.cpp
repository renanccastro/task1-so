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
pthread_mutex_t num_bats_mutex;
int num_bats;


// Batman
void *batman(void *arg){
	while(){
		
	}
}

// Trem vindo do norte
void *north_bat(void *arg) {
	char * inputString = (char*)arg;
	int queue_size;
	for (int i = 0; i < strlen(inputString); i++)
	{
		if (inputString[i] == 'n')
		{
			//adiciona na queue
			north_queue.push(i+1);
			printf("Adicionou BAT %d na fila\n", i+1);

		}
	}
	queue_size = north_queue.size();
	for (int i = 0; i < queue_size(); i++)
	{
		int batNumber = north_queue.pop();
		printf("BAT %d N chegou no cruzamento\n", batNumber);
	}
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

	// Threads para cada direcao
	pthread_t batid[4];

	// Inicia o mutex para acesso ao numero total de bats
	pthread_mutex_init(&num_bats_mutex, NULL);


	// Inicia o semaforo de cada direção
	sem_init(&north_sem, 0, 10);

	//inicializa as threads por queue/direcao
	pthread_create(&batid[0], NULL, north_bat, argv[1]);
	pthread_join(batid[0], NULL);
	// pthread_create(&batid[1], NULL, south_bat, &argv[1]);
	// pthread_create(&batid[2], NULL, east_bat, &argv[1]);
	// pthread_create(&batid[3], NULL, west_bat, &argv[1]);

	// for (i = 0; i < numBat; i++) {
	// 	if (argv[1][i] == 'n') {
	// 		if (pthread_create(&batid[i], NULL, north_bat, &i)) {
	// 			printf("Erro ao criar thread %d\n", i);
	// 		}
	// 	} 
	// 	*
	// 	  else if (argv[1][i] == 'e') {
	// 	  pthread_create(&batid[i], NULL, east_bat, i); 
	// 	  } 
	// 	  else if (argv[1][i] == 'w') {
	// 	  pthread_create(&batid[i], NULL, west_bat, i); 
	// 	  } 
	// 	  else if (argv[1][i] == 's') {
	// 	  pthread_create(&batid[i], NULL, south_bat, i); 
	// 	  } 
	// 	 *
	// 	else {
	// 		printf("Direção inválida - ['n','s','e','w']\n");
	// 	} 
	// }
	// for (i = 0; i < numBat; i++) {
	// 		if (pthread_join(batid[i], NULL)) {
	// 			printf("Erro join thread %d\n", i);
	// 		}
	// }
	
	// int tam = north_queue.size();
	// for (i = 0; i < tam; i++) {
	// 	printf("(%d) BAT %d\n", i, north_queue.front());
	// 	north_queue.pop();
	// }

	return 0;
}

