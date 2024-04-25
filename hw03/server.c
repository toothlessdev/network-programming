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
void getPosition(int *row, int *col, const GAMEBOARD *board);
int isInBoundary(const int row, const int col);
int available_space(const GAMEBOARD *board);

int main(int argc, char *argv[]){
    srand(time(NULL));

    int server_socket;
    socklen_t client_addr_size;
    struct sockaddr_in server_addr, client_addr;

    if(argc != 2) {
        printf("Usage : %s <PORT>\n", argv[0]);
        exit(1);
    }

    server_socket = socket(PF_INET, SOCK_DGRAM, 0); // SOCK_DGRAM : UDP
    if(server_socket == -1) err("UDP Socket creation error!");

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) err("bind() error!");

    GAMEBOARD board;
    for(int row = 0; row < BOARD_SIZE; row++) {
        for(int col = 0; col < BOARD_SIZE; col++){
            board.board[row][col] = INIT_VALUE;
        }
    }

    printf("Tik-Tak-Toe Server\n");
    printBoard(&board);

    while(1) {
        client_addr_size = sizeof(client_addr);
        recvfrom(server_socket, &board, sizeof(GAMEBOARD), 0, (struct sockaddr*)&client_addr, &client_addr_size);

        printBoard(&board);
        
        if(available_space(&board) == 0) {
            printf("No available Space. Exit this program\n");
            break;
        }
        
        int row = 0, col = 0;
        getPosition(&row, &col, &board);
        board.board[row][col] = S_VALUE;

        printf("Server Choose: [%d, %d]\n", row, col);
        printBoard(&board);

        sendto(server_socket, &board, sizeof(GAMEBOARD), 0, (struct sockaddr*)&client_addr, client_addr_size);

        printf("\n");
    }

    close(server_socket);
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

void getPosition(int *row, int *col, const GAMEBOARD *board) {
    int _row = rand() % 3;
    int _col = rand() & 3;

    if(board->board[_row][_col] != INIT_VALUE) {
        getPosition(row, col, board);
    }
    else {
        *row = _row;
        *col = _col;
        return;
    }
}