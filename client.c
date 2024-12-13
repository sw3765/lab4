#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"  // 서버 IP 주소
#define PORT 8080
#define MAX_BUFFER 1024

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[MAX_BUFFER];

    // 1. 소켓 생성
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket failed");
        return -1;
    }

    // 2. 서버 주소 설정
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // 3. 서버에 연결
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connect failed");
        return -1;
    }
    printf("Connected to server\n");

    // 4. 메시지 전송
    printf("Enter message: ");
    fgets(buffer, sizeof(buffer), stdin);
    send(sock, buffer, strlen(buffer), 0);

    // 5. 응답 받기
    memset(buffer, 0, sizeof(buffer));
    recv(sock, buffer, sizeof(buffer), 0);
    printf("Server response: %s\n", buffer);

    // 6. 소켓 종료
    close(sock);

    return 0;
}
