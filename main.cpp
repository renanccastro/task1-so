#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <queue>
#include <vector>

// Define o valor de K a partir do qual passa a ceder
#define K 3
//

typedef enum direction {
	NORTH, EAST, SOUTH, WEST
} direction;

using namespace std;

//* Variaveis globais

// Fila de prioridade
queue<int> queues[4];

// Bitmask
bool mask[4];
bool givein_mask[4];

//Mutexes
pthread_mutex_t queue_mutex[4]; // Mutexes para cada fila
pthread_mutex_t mask_mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex para acesso à bitmask
pthread_mutex_t givein_mask_mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex para acesso à quem cedeu

// Semaphores
sem_t queue_sem[4]; // Semaphore para cada fila
sem_t givein_sem[4]; // Semaphore para cada fila

void *bat(void *arg);
// Obtem número de bats de cada fila
void getBats(int* num_bats) {
	for (int i = 0; i < 4; ++i)
	{
		pthread_mutex_lock(&queue_mutex[i]);
		num_bats[i] = queues[i].size();
		pthread_mutex_unlock(&queue_mutex[i]);
	}
}
// Obtem os bats que cederam
void getGiveIn(int* givein_bitmask_local ) { 
	pthread_mutex_lock(&givein_mask_mutex);
	for(int a = 0; a < 4; a++){
		givein_bitmask_local[a] = givein_mask[a];
	}
	pthread_mutex_unlock(&givein_mask_mutex);
}
// Obtem a soma de um vetor
int sumVector(int vec[], int tam) {
	int i;
	int sum = 0;
	for (i = 0 ; i < tam; i++) {
		sum += vec[i];
	}
	return sum;
}

// Retorna o caracter da direção
char dirName(int e) {
	if (e == NORTH) return 'N';
	else if (e == EAST) return 'E';
	else if (e == SOUTH) return 'S';
	else return 'W';
}

// Batman
void *batman(void *arg){
	int i; // contador
	int num_bats[4]; // Numero de bats esperando em cada direção
	int tot_bats;

	pthread_t batid[4]; // Vetor para cada thread

	// Soma o numero de elementos em cada posição
	getBats(num_bats);
	tot_bats = sumVector(num_bats, 4);

	// Cria as threads nas direçoes que tem bats
	for (i = 0; i < 4; i++)
	{
		if(num_bats[i] > 0){
			int* j = (int*)malloc(sizeof(int));
			*j = i;
			pthread_create(&batid[i], NULL, bat, j);
		}
	}

	//adquire os semaforos
	/*	for(int h = NORTH; h < 4; h++){
		sem_wait(&queue_sem[h]);
		}
	 */
	//semaforos em 0, qualquer chamada a wait vai bloquear a thread
	do {
		// Obtem o número de bats em cada fila
		getBats(num_bats); 
		int next_direction;
		tot_bats = sumVector(num_bats, 4);
		int givein_bitmask_local[4];
		getGiveIn(givein_bitmask_local); 
		//Escolhe a direcao com base na prioridade. 
		if(num_bats[NORTH] > 0 && !givein_bitmask_local[NORTH])
			i = NORTH;
		else if(num_bats[EAST] > 0 && !givein_bitmask_local[EAST]) 
			i = EAST;
		else if(num_bats[SOUTH] > 0 && !givein_bitmask_local[SOUTH])
			i = SOUTH;
		else if(num_bats[WEST] > 0 && !givein_bitmask_local[WEST])
			i = WEST;
		else 
			break;
		// Se algum cedeu, tira para ser chamado na proxima vez
		for (int p = 0 ; p < 4; p++) {
			if (givein_bitmask_local[p]) {
				// Tira a mask dos que cedeu
				pthread_mutex_lock(&givein_mask_mutex);
				givein_mask[p] = false;
				pthread_mutex_unlock(&givein_mask_mutex);
			}
		}
		// Verifica se tem impasse com outra direção
		if (num_bats[i] > K) {	
			// Sinaliza para prosseguir
			sem_post(&queue_sem[i]);
			// Aguarda ou ceder
			sem_wait(&givein_sem[i]);
			// Cedeu, então chama o próximo no proximo while
		}
		else {
			//Como ninguém foi sinalizado, num_bats ainda é o mesmo
			for (int j = i + 1; j < 4; j++) {
				next_direction = j; 
				//printf("this = %c, next = %c\n", dirName(j), dirName(next_direction));
				// Verifica o impasse
				pthread_mutex_lock(&mask_mutex); 
				if (mask[next_direction] == true ) { 
					// Impasse entre i e next_direction
					// Imprime que deu impasse
					printf("Impasse: %c e %c, sinalizando %c para ir\n", dirName(i),dirName(next_direction),dirName(i));                
					pthread_mutex_unlock(&mask_mutex);
					break;
				}
				pthread_mutex_unlock(&mask_mutex);
			}

			// Sinaliza para prosseguir
			//printf("Enviou sinal pra thread %c\n", dirName(i));
			sem_post(&queue_sem[i]);
			// Aguarda terminar
			sem_wait(&givein_sem[i]);
		}

	} while(tot_bats > 0);
	pthread_exit(NULL);
}

// Trem vindo do norte
// Cada vez que for executado chega na plataforma, aguarda batman chamar e imprime
void *bat(void *arg) {
	int bat_direction = *(int*)arg; // Direção do Bat
	int queue_size; // Tamanho da fila naquela direção
	//sem_wait(&givein_sem[bat_direction]); // Zera o semaforo
	do {
		// Obtem o primeiro da fila
		pthread_mutex_lock(&queue_mutex[bat_direction]); // Obtem acesso exclusivo à fila de prioridades
		int batNumber = queues[bat_direction].front(); // Obtem número do primeiro da fila
		queue_size = queues[bat_direction].size(); // Obtem tamanho da nova queue
		// Imprime que chegou no cruzamento
		printf("BAT %d %c chegou no cruzamento\n", batNumber, dirName(bat_direction));
		pthread_mutex_unlock(&queue_mutex[bat_direction]); // Libera acesso
		// Atualiza mask
		pthread_mutex_lock(&mask_mutex);
		mask[bat_direction] = true;
		pthread_mutex_unlock(&mask_mutex);

		// Aguarda ser chamado pelo BATMAN
		sem_wait(&queue_sem[bat_direction]);
		//printf("Thread recebeu sinal do batman\n");

		// Verifica se vai ceder a vez
		if(queue_size > K) {
			// Se existe algum bat em outra direcao
			for (int j = bat_direction + 1; j < 4; j++) {
				pthread_mutex_lock(&mask_mutex); 
				if (mask[j] == true ) { 
					// Obtem acesso à fila a quem irá ceder
					pthread_mutex_lock(&queue_mutex[j]); // Obtem acesso exclusivo à fila de prioridades
					int givein = queues[j].front(); // Obtem a quem irá ceder
					// Imprime que cedeu
					printf("BAT %d %c cedeu passagem BAT %d %c\n", batNumber, dirName(bat_direction), givein, dirName(j));
					//setamos  a máscara de que cedeu
					pthread_mutex_lock(&givein_mask_mutex);
					givein_mask[bat_direction] = true;
					pthread_mutex_unlock(&givein_mask_mutex);
					pthread_mutex_unlock(&queue_mutex[j]); // Libera acesso
					pthread_mutex_unlock(&mask_mutex); // Libera mask
					break;
				}
				pthread_mutex_unlock(&mask_mutex); // Libera mask

			}
			// Passa a vez
			sem_post(&givein_sem[bat_direction]);
			// Libera a fila e espera novamente ser chamado pois nao pode mais passar a vez
			sem_wait(&queue_sem[bat_direction]);
		}
		// Atravessa a plataforma
		// Obtem acesso e retira da lista
		pthread_mutex_lock(&queue_mutex[bat_direction]);
		queues[bat_direction].pop(); // Retira da fila
		queue_size = queues[bat_direction].size(); // Obtem tamanho da nova queue
		// Quando for chamado imprime que saiu
		printf("BAT %d %c saiu do cruzamento\n", batNumber, dirName(bat_direction));
		pthread_mutex_unlock(&queue_mutex[bat_direction]);
		// Atualiza mask
		pthread_mutex_lock(&mask_mutex);
		mask[bat_direction] = false;
		pthread_mutex_unlock(&mask_mutex);
		// Ao terminar libera o batman
		sem_post(&givein_sem[bat_direction]);
	} while(queue_size);
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
	for (int i = 0; i < numBat; i++) {
		if (argv[1][i] == 'n') {
			//adiciona na queue
			queues[NORTH].push(i+1);
		}else if(argv[1][i] == 's'){
			//adiciona na queue
			queues[SOUTH].push(i+1);

		}else if(argv[1][i] == 'w'){
			//adiciona na queue
			queues[WEST].push(i+1);

		}else if(argv[1][i] == 'e'){
			//adiciona na queue
			queues[EAST].push(i+1);
		}else {
			printf("Direção inválida - ['n','s','e','w']\n");
			return 0;
		}
	}
	// Threads para o batman
	pthread_t batmanager;

	// Inicia o mutex para acesso ao numero total de bats
	//	pthread_mutex_init(&num_bats_mutex, NULL);
	// Inicia os mutexes para acessa à fila e semaforos
	for( int i = 0; i < 4; i++){
		sem_init(&queue_sem[i], 0, 1); // Inicia semaforo para passagem de bat (retirada da fila)
		sem_wait(&queue_sem[i]);
		sem_init(&givein_sem[i], 0, 1); // Inicia semaforo para avisar o batman quem cedeu
		sem_wait(&givein_sem[i]);
		pthread_mutex_init(&queue_mutex[i], NULL); // Mutex para acesso exclusivo à fila
		mask[i] = false; // Inicia como false
		givein_mask[i] = false; // Inicia como false
	}

	//inicializa as threads por queue/direcao
	pthread_create(&batmanager, NULL, batman, NULL);
	pthread_join(batmanager, NULL);

	return 0;
}