#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_BUFFER 1024

int main() {
    int server_fd, new_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[MAX_BUFFER];

    // 1. 소켓 생성
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket failed");
        return -1;
    }

    // 2. 서버 주소 설정
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;  // 모든 인터페이스에서 수신
    server_addr.sin_port = htons(PORT);  // 포트 설정

    // 3. 소켓에 주소 바인딩
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        return -1;
    }

    // 4. 연결 대기
    if (listen(server_fd, 3) == -1) {
        perror("Listen failed");
        return -1;
    }
    printf("Server listening on port %d...\n", PORT);

    // 5. 클라이언트 연결 받기
    if ((new_sock = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len)) == -1) {
        perror("Accept failed");
        return -1;
    }

    printf("Client connected\n");

    // 6. 데이터 수신 및 전송
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(new_sock, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            printf("Client disconnected\n");
            break;
        }
        printf("Received from client: %s\n", buffer);

        // 응답 전송
        send(new_sock, "Message received", 16, 0);
    }

    // 7. 소켓 종료
    close(new_sock);
    close(server_fd);

    return 0;
}
