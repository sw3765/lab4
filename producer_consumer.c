#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFFER_SIZE 5

int buffer[BUFFER_SIZE];
int count = 0;
pthread_mutex_t mutex;
pthread_cond_t not_full, not_empty;

// 생산자 함수
void* producer(void* arg) {
    int id = *(int*)arg;
    while (1) {
        pthread_mutex_lock(&mutex);
        while (count == BUFFER_SIZE) { // 버퍼가 꽉 찼을 때 대기
            pthread_cond_wait(&not_full, &mutex);
        }
        int item = rand() % 100;
        buffer[count++] = item;
        printf("Producer %d: Produced %d\n", id, item);
        pthread_cond_signal(&not_empty); // 소비자 깨움
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
    return NULL;
}

// 소비자 함수
void* consumer(void* arg) {
    int id = *(int*)arg;
    while (1) {
        pthread_mutex_lock(&mutex);
        while (count == 0) { // 버퍼가 비었을 때 대기
            pthread_cond_wait(&not_empty, &mutex);
        }
        int item = buffer[--count];
        printf("Consumer %d: Consumed %d\n", id, item);
        pthread_cond_signal(&not_full); // 생산자 깨움
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t producers[2], consumers[2];
    int ids[2] = {1, 2};

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&not_full, NULL);
    pthread_cond_init(&not_empty, NULL);

    // 생산자와 소비자 쓰레드 생성
    for (int i = 0; i < 2; i++) {
        pthread_create(&producers[i], NULL, producer, &ids[i]);
        pthread_create(&consumers[i], NULL, consumer, &ids[i]);
    }

    // 쓰레드 종료 대기
    for (int i = 0; i < 2; i++) {
        pthread_join(producers[i], NULL);
        pthread_join(consumers[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&not_full);
    pthread_cond_destroy(&not_empty);

    return 0;
}
