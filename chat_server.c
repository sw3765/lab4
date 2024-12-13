#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

int client_sockets[MAX_CLIENTS];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// 클라이언트 메시지 브로드캐스트
void broadcast_message(char *message, int sender_socket) {
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] != 0 && client_sockets[i] != sender_socket) {
            send(client_sockets[i], message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&mutex);
}

// 클라이언트 처리 쓰레드
void *handle_client(void *arg) {
    int client_socket = *(int *)arg;
    char buffer[BUFFER_SIZE];
    int bytes_read;

    while ((bytes_read = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        buffer[bytes_read] = '\0';  // Null-terminate the string
        printf("Client %d: %s\n", client_socket, buffer);
        broadcast_message(buffer, client_socket);
    }

    // 클라이언트 연결 종료
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] == client_socket) {
            client_sockets[i] = 0;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);

    printf("Client %d disconnected.\n", client_socket);
    close(client_socket);
    free(arg);
    return NULL;
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // 클라이언트 소켓 배열 초기화
    memset(client_sockets, 0, sizeof(client_sockets));

    // 서버 소켓 생성
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // 소켓 바인딩
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // 연결 대기
    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    while (1) {
        // 클라이언트 연결 수락
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
        if (client_socket == -1) {
            perror("Accept failed");
            continue;
        }

        printf("New client connected: %d\n", client_socket);

        // 클라이언트 소켓 배열에 추가
        pthread_mutex_lock(&mutex);
        int added = 0;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] == 0) {
                client_sockets[i] = client_socket;
                added = 1;
                break;
            }
        }
        pthread_mutex_unlock(&mutex);

        if (!added) {
            printf("Max clients reached. Connection refused: %d\n", client_socket);
            close(client_socket);
            continue;
        }

        // 클라이언트 처리 쓰레드 생성
        pthread_t thread_id;
        int *arg = malloc(sizeof(int));
        *arg = client_socket;
        pthread_create(&thread_id, NULL, handle_client, arg);
        pthread_detach(thread_id);
    }

    close(server_socket);
    return 0;
}
