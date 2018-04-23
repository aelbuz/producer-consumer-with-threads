#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 5

typedef int buffer_item;
sem_t full, empty; // 'boş' ve 'dolu' semaforları
pthread_mutex_t mutex; // Mutex semaforu
buffer_item buffer[BUFFER_SIZE]; // Buffer
int bufferCounter; // Buffer counter
pthread_t tid;
pthread_attr_t attr;

int insert_item(buffer_item item);
int remove_item(buffer_item *item);

void *producer(void *param) // Producer thread
{
	buffer_item item;

	while (1)
	{
		int randNum = rand() / 100000000; // Sleep for a random period of time
		sleep(randNum);

		item = rand();

		sem_wait(&empty); // Wait
		pthread_mutex_lock(&mutex); // Lock

		if (insert_item(item))
		{
			fprintf(stderr, "Producer error\n");
		}
		else
		{
			printf("Producer produced: %d\n", item);
		}

		pthread_mutex_unlock(&mutex); // Unlock
		sem_post(&full); // Signal
	}
}

void *consumer(void *param) // Consumer thread
{
	buffer_item item;

	while (1)
	{
		int randNum = rand() / 100000000; // Sleep for a random period of time
		sleep(randNum);

		sem_wait(&full); // Wait
		pthread_mutex_lock(&mutex); // Lock

		if (remove_item(&item))
		{
			fprintf(stderr, "Consumer error\n");
		}
		else
		{
			printf("Consumer consumed: %d\n", item);
		}

		pthread_mutex_unlock(&mutex); // Unlock
		sem_post(&empty); // Signal
	}
}

int insert_item(buffer_item item) // Buffer'a veri yaz
{
	if (bufferCounter < BUFFER_SIZE) // Buffer dolu değilse veriyi yaz
	{
		buffer[bufferCounter] = item;
		bufferCounter++;
		return 0;
	}
	else // Buffer dolu ise hata ver
	{
		return -1;
	}
}

int remove_item(buffer_item *item) // Buffer'dan veri al
{
	if (bufferCounter > 0) // Buffer boş değilse veriyi al
	{
		*item = buffer[(bufferCounter - 1)];
		bufferCounter--;
		return 0;
	}
	else // Buffer boş ise hata ver
	{
		return -1;
	}
}

int main(int argc, char *argv[])
{
	int i, j;
	int sleepMain = atoi(argv[1]); // main fonksiyonunun uyuma zamanı
	int producerNum = atoi(argv[2]); // Producer thread sayısı
	int consumerNum = atoi(argv[3]); // Consumer thread sayısı

	pthread_mutex_init(&mutex, NULL); // Mutex oluşturma
	sem_init(&full, 0, 0); // 'full' semaforunu yükle ve 0 ata
	sem_init(&empty, 0, BUFFER_SIZE); // 'empty' semaforunu yükle ve buffer boyutunu ata
	pthread_attr_init(&attr);
	bufferCounter = 0; // Initialize buffer

	for (i = 0; i < producerNum; i++) // Producer thread oluşturma
	{
		pthread_create(&tid, &attr, producer, NULL);
	}

	for (j = 0; j < consumerNum; j++) // Consumer thread oluşturma
	{
		pthread_create(&tid, &attr, consumer, NULL);
	}

	sleep(sleepMain);
	exit(0);
}
