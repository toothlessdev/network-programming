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

char arr[10000000];
char size = 0;

void err(char *message);
void initBoard(char board[BOARD_SIZE][BOARD_SIZE]);
void fillBoard(char board[BOARD_SIZE][BOARD_SIZE]);
void printBoard(char board1[BOARD_SIZE][BOARD_SIZE], char board2[BOARD_SIZE][BOARD_SIZE]);
int compareBoard(char board[BOARD_SIZE][BOARD_SIZE], char ret[BOARD_SIZE][BOARD_SIZE], char arr[], int size);
int isFullyFilled(char board[BOARD_SIZE][BOARD_SIZE]);
int isAlreadyChecked(char arr[], int size, char ch);

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

    REQ_PACKET req_packet;
    RES_PACKET res_packet;
    memset(&req_packet, 0, sizeof(REQ_PACKET));
    memset(&res_packet, 0, sizeof(RES_PACKET));

    printf("------------------------------\n");
    printf("Finding Alphabet Game Server\n");
    printf("------------------------------\n");

    // [1] 5x5 2D Array Init
    char board[BOARD_SIZE][BOARD_SIZE];
    fillBoard(board);
    initBoard(res_packet.board);

    printBoard(board, res_packet.board);

    while(1) {
        client_addr_size = sizeof(client_addr);
        recvfrom(server_socket, &req_packet, sizeof(req_packet), 0, (struct sockaddr*)&client_addr, &client_addr_size);
        
        if(req_packet.cmd == GAME_REQ) {
            printf("[Server] Rx cmd=%d, ch=%c\n", req_packet.cmd, req_packet.ch);
            arr[size++] = req_packet.ch;


            int match = compareBoard(board, res_packet.board, arr, size);
            printBoard(board, res_packet.board);
            

            if(isFullyFilled(res_packet.board)) {
                res_packet.cmd = GAME_END;
                res_packet.result = 0;
                sendto(server_socket, &res_packet, sizeof(res_packet), 0, (struct sockaddr*)&client_addr, client_addr_size);
                printf("No Empty Space. Exit this program\n");
                printf("[Server] Tx cmd=%d, result=%d\n", res_packet.cmd, res_packet.result);
                break;
            } else {
                res_packet.cmd = GAME_RES;
                res_packet.result = match;
                sendto(server_socket, &res_packet, sizeof(res_packet), 0, (struct sockaddr*)&client_addr, client_addr_size);
                printf("[Server] Tx cmd=%d, result=%d\n", res_packet.cmd, res_packet.result);
            }
        }
    }

    close(server_socket);
}

void err(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void initBoard(char board[BOARD_SIZE][BOARD_SIZE]){
    for(int y = 0; y < BOARD_SIZE ; y++) {
        for(int x = 0; x < BOARD_SIZE; x++) {
            board[y][x] = ' ';
        }
    }
}
void fillBoard(char board[BOARD_SIZE][BOARD_SIZE]) {
    for(int y = 0; y < BOARD_SIZE ; y++) {
        for(int x = 0; x < BOARD_SIZE; x++) {
            board[y][x] = 'A' + (rand() % 26);
        }
    }
}
void printBoard(char board1[BOARD_SIZE][BOARD_SIZE], char board2[BOARD_SIZE][BOARD_SIZE]) {
    printf("+-------------------+	+-------------------+\n");
    for(int row = 0; row < BOARD_SIZE; row++) {
        for(int col = 0; col < BOARD_SIZE ; col++) {
            printf("| %c ", board1[row][col]);
        }
        printf("|   ");
        for(int col = 0; col < BOARD_SIZE; col++) {
            printf("| %c ", board2[row][col]);
        }
        printf("|\n");
    }
    printf("+-------------------+	+-------------------+\n");
}
int compareBoard(char board[BOARD_SIZE][BOARD_SIZE], char ret[BOARD_SIZE][BOARD_SIZE], char arr[], int size) {
    // for(int i = 0; i < size; i++) printf("%c ", arr[i]);
    // printf("\n");

    int match = 0;
    for(int row = 0; row < BOARD_SIZE; row++) {
        for(int col = 0; col < BOARD_SIZE; col++) {
            for(int index = 0; index < size ;index++) {

                if(board[row][col] == arr[index]) {
                    ret[row][col] = board[row][col];
                    if(isAlreadyChecked(arr, size-1, arr[index]) == 0) match++;
                }
            }
        }
    }
    return match;
}
int isFullyFilled(char board[BOARD_SIZE][BOARD_SIZE]) {
    for(int row = 0; row < BOARD_SIZE ; row++){
        for(int col = 0; col < BOARD_SIZE; col++) {
            if(board[row][col] == ' ') return 0;
        }
    }
    return 1;
}
int isAlreadyChecked(char arr[], int size, char ch) {
    for(int index = 0; index < size; index++) {
        if(arr[index] == ch) return 1;
    }
    return 0;
}