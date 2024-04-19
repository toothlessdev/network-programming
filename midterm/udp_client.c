// 2020113486 김대건

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<time.h>

#define BOARD_SIZE 5
#define GAME_REQ 0
#define GAME_RES 1
#define GAME_END 2

typedef struct {
    int cmd;
    char ch;
} REQ_PACKET;

typedef struct {
    int cmd;
    char board[BOARD_SIZE][BOARD_SIZE];
    int result;
} RES_PACKET;

void err(char *message);
void printBoard(char board[BOARD_SIZE][BOARD_SIZE]);

int main(int argc, char *argv[]) {
    srand(time(NULL));

    int sock;
    int str_len;
    socklen_t addr_size;
    struct sockaddr_in server_addr, from_addr;

    if(argc != 3) {
        printf("Usage : %s <IP> <PORT>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock == -1) err("socket() error");

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    REQ_PACKET req_packet;
    RES_PACKET res_packet;

    while(1) {
        req_packet.cmd = GAME_REQ;
        req_packet.ch = 'A' + (rand() % 26);
        sendto(sock, &req_packet, sizeof(REQ_PACKET), 0,  (struct sockaddr*)&server_addr, sizeof(server_addr));
        printf("[Client] Tx cmd=%d, ch=%c\n", req_packet.cmd, req_packet.ch);

        addr_size = sizeof(from_addr);
        recvfrom(sock, &res_packet, sizeof(RES_PACKET), 0, (struct sockaddr*)&from_addr, &addr_size);

        if(res_packet.cmd == GAME_RES) {
            printBoard(res_packet.board);
            printf("[Client] Rx cmd=%d, result=%d\n", res_packet.cmd, res_packet.result);
        }
        else if(res_packet.cmd == GAME_END) {
            printBoard(res_packet.board);
            printf("[Client] Rx cmd=%d, result=%d\n", res_packet.cmd, res_packet.result);
            printf("No Empty Space. Exit this program.\n");
            break;
        }
        sleep(1);
    }

    close(sock);
    return 0;
}

void err(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
void printBoard(char board[BOARD_SIZE][BOARD_SIZE]) {
    printf("+-------------------+\n");
    for(int row = 0; row < BOARD_SIZE; row++) {
        for(int col = 0; col < BOARD_SIZE ; col++) {
            printf("| %c ", board[row][col]);
        }
        printf("|\n");
    }
    printf("+-------------------+\n");
}
