#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_CLIENTS 5
#define MAX_MESSAGE_LENGTH 256

char buffer[MAX_MESSAGE_LENGTH];
int message_available = 0; // 메시지가 준비되었는지 여부

pthread_mutex_t mutex;
pthread_cond_t cond_server, cond_clients;

// 서버 쓰레드: 메시지를 수신하고 방송
void* server_thread(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        // 메시지가 들어올 때까지 대기
        while (!message_available) {
            pthread_cond_wait(&cond_server, &mutex);
        }

        // 메시지를 방송
        printf("[Server] Broadcasting message: %s\n", buffer);

        // 모든 클라이언트에게 알림
        message_available = 0;
        pthread_cond_broadcast(&cond_clients);

        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

// 클라이언트 쓰레드: 서버에 메시지 전송 요청
void* client_thread(void* arg) {
    int client_id = *(int*)arg;
    char message[MAX_MESSAGE_LENGTH];

    while (1) {
        sleep(rand() % 5 + 1); // 랜덤 시간 대기
        snprintf(message, MAX_MESSAGE_LENGTH, "Message from Client %d", client_id);

        pthread_mutex_lock(&mutex);

        // 메시지 버퍼가 비어있을 때만 작성 가능
        while (message_available) {
            pthread_cond_wait(&cond_clients, &mutex);
        }

        strcpy(buffer, message);
        message_available = 1;
        printf("[Client %d] Sent message: %s\n", client_id, message);

        // 서버에게 알림
        pthread_cond_signal(&cond_server);

        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    pthread_t server;
    pthread_t clients[MAX_CLIENTS];
    int client_ids[MAX_CLIENTS];

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_server, NULL);
    pthread_cond_init(&cond_clients, NULL);

    // 서버 쓰레드 생성
    pthread_create(&server, NULL, server_thread, NULL);

    // 클라이언트 쓰레드 생성
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_ids[i] = i + 1;
        pthread_create(&clients[i], NULL, client_thread, &client_ids[i]);
    }

    // 클라이언트 쓰레드 종료 대기
    for (int i = 0; i < MAX_CLIENTS; i++) {
        pthread_join(clients[i], NULL);
    }

    pthread_cancel(server);
    pthread_join(server, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_server);
    pthread_cond_destroy(&cond_clients);

    return 0;
}

