// 2020113486 김대건

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<time.h>

#define BOARD_SIZE 3
#define INIT_VALUE 0
#define S_VALUE 1
#define C_VALUE 2

typedef struct {
    int board[BOARD_SIZE][BOARD_SIZE];
} GAMEBOARD;

void err(char *message);
void printBoard(const GAMEBOARD *board);
int isInBoundary(const int row, const int col);
int available_space(const GAMEBOARD *board);

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

    GAMEBOARD board;
    for(int row = 0; row < BOARD_SIZE; row++) {
        for(int col = 0; col < BOARD_SIZE; col++){
            board.board[row][col] = INIT_VALUE;
        }
    }

    printf("Tik-Tak-Toe Client\n");
    printBoard(&board);

    while(1) {
        int row, col;
        printf("Input row, column : ");
        scanf("%d %d", &row, &col);

        if(!(isInBoundary(row, col) && board.board[row][col] == INIT_VALUE)) continue;
        board.board[row][col] = C_VALUE;
        printBoard(&board);
        
        sendto(sock, &board, sizeof(GAMEBOARD), 0,(struct sockaddr*)&server_addr, sizeof(server_addr));

        if(available_space(&board) == 0) {
            printf("No available Space. Exit this program\n");
            break;
        }

        addr_size = sizeof(from_addr);
        recvfrom(sock, &board, sizeof(GAMEBOARD), 0, (struct sockaddr*)&from_addr, &addr_size);
        printBoard(&board);

        printf("\n");
    }

    close(sock);
    return 0;
}

void err(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void printBoard(const GAMEBOARD *board) {
    char value = ' ';
    printf("+-----------+\n");
    
    for(int i = 0; i < BOARD_SIZE ; i++ ){
        for(int j = 0; j < BOARD_SIZE; j++) {
            if(board->board[i][j] == INIT_VALUE) value = ' ';
            else if(board->board[i][j] == S_VALUE) value = 'O';
            else if(board->board[i][j] == C_VALUE) value = 'X';
            else value = ' ';
            printf("| %c ", value);
        }
        printf("|");
        printf("\n+-----------+\n");
    }
}

int available_space(const GAMEBOARD *board) {
    for(int row = 0; row < BOARD_SIZE ; row++) {
        for(int col = 0; col < BOARD_SIZE ; col++) {
            if(board->board[row][col] == 0) return 1;
        }
    }
    return 0;
}

int isInBoundary(const int row, const int col) {
    return 0 <= row && row < BOARD_SIZE && 0 <= col && col < BOARD_SIZE;
}