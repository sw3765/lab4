#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8081
#define BUFFER_SIZE 4096

void handle_request(int client_socket) {
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    char method[16], path[256];

    // 요청 읽기
    int bytes_read = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_read <= 0) {
        perror("Failed to read request");
        close(client_socket);
        return;
    }
    buffer[bytes_read] = '\0'; // Null-terminate the buffer

    // 요청 메서드와 경로 파싱
    sscanf(buffer, "%s %s", method, path);

    // POST 요청 처리
    if (strcmp(method, "POST") == 0) {
        char *body = strstr(buffer, "\r\n\r\n"); // 요청 본문 시작점 찾기
        if (body) {
            body += 4; // 본문 시작 위치로 이동
            printf("Received POST data:\n%s\n", body);

            // 간단한 응답 생성
            sprintf(response,
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/plain\r\n"
                    "Content-Length: 15\r\n"
                    "\r\nPOST received!");
        } else {
            sprintf(response,
                    "HTTP/1.1 400 Bad Request\r\n"
                    "Content-Type: text/plain\r\n"
                    "Content-Length: 12\r\n"
                    "\r\nBad Request");
        }
    } else {
        // 기타 요청 처리 (예: GET)
        sprintf(response,
                "HTTP/1.1 405 Method Not Allowed\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: 23\r\n"
                "\r\nMethod Not Allowed");
    }

    // 응답 전송
    send(client_socket, response, strlen(response), 0);
    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

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
    if (listen(server_socket, 5) == -1) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server is running on port %d...\n", PORT);

    while (1) {
        // 클라이언트 연결 수락
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
        if (client_socket == -1) {
            perror("Accept failed");
            continue;
        }

        // 요청 처리
        handle_request(client_socket);
    }

    close(server_socket);
    return 0;
}
