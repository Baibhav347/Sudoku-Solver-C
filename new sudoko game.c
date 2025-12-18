#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

// --- PART 1: SUDOKU SOLVING LOGIC ---

int isSafe(int board[9][9], int row, int col, int num) {
    for (int x = 0; x <= 8; x++) if (board[row][x] == num) return 0;
    for (int x = 0; x <= 8; x++) if (board[x][col] == num) return 0;
    int startRow = row - row % 3, startCol = col - col % 3;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (board[i + startRow][j + startCol] == num) return 0;
    return 1;
}

int solveSudokuAlgorithm(int board[9][9], int row, int col) {
    if (row == 8 && col == 9) return 1;
    if (col == 9) { row++; col = 0; }
    if (board[row][col] > 0) return solveSudokuAlgorithm(board, row, col + 1);
    for (int num = 1; num <= 9; num++) {
        if (isSafe(board, row, col, num)) {
            board[row][col] = num;
            if (solveSudokuAlgorithm(board, row, col + 1)) return 1;
        }
        board[row][col] = 0;
    }
    return 0;
}

void solve_sudoku_handler(char *input_str, char *output_str) {
    int grid[9][9];
    int k = 0;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            grid[i][j] = input_str[k++] - '0';
        }
    }
    if (solveSudokuAlgorithm(grid, 0, 0)) {
        k = 0;
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                output_str[k++] = grid[i][j] + '0';
            }
        }
        output_str[81] = '\0';
    } else {
        strcpy(output_str, "ERROR: No Solution Found");
    }
}

// --- PART 2: SERVER LOGIC ---

int main() {
    WSADATA wsa;
    SOCKET server_fd, new_socket;
    struct sockaddr_in server, client;
    int c;
    char buffer[2000];
    char solution[100];      // <--- This is where 'solution' is created
    char http_response[2000];

    printf("Initializing Winsock...\n");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
        printf("Failed. Error Code : %d",WSAGetLastError());
        return 1;
    }

    if((server_fd = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET) {
        printf("Could not create socket : %d" , WSAGetLastError());
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8080 );

    if( bind(server_fd ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed with error code : %d" , WSAGetLastError());
    }

    listen(server_fd , 3);
    printf("Waiting for incoming connections on http://localhost:8080 ...\n");

    c = sizeof(struct sockaddr_in);

    // --- THE MAIN LOOP STARTS HERE ---
    while( (new_socket = accept(server_fd , (struct sockaddr *)&client, &c)) != INVALID_SOCKET ) {

        int read_size = recv(new_socket , buffer , 2000 , 0);
        buffer[read_size] = '\0';

        char *puzzle_start = strstr(buffer, "?board=");

        if (puzzle_start != NULL) {
            puzzle_start += 7;
            char puzzle_data[82];  // <--- This is where 'puzzle_data' is created
            strncpy(puzzle_data, puzzle_start, 81);
            puzzle_data[81] = '\0';

            printf("Solving Puzzle...\n");

            // CALL THE SOLVER (This works now because we are inside the loop!)
            solve_sudoku_handler(puzzle_data, solution);

            // Print the solution to your black window
            printf("DONE! Sent solution: %s\n", solution);

            sprintf(http_response,
                "HTTP/1.1 200 OK\n"
                "Content-Type: text/plain\n"
                "Access-Control-Allow-Origin: *\n"
                "\n"
                "%s", solution);
        } else {
             sprintf(http_response, "HTTP/1.1 404 Not Found\n\n");
        }

        send(new_socket , http_response , strlen(http_response) , 0);
        closesocket(new_socket);
    }
    // --- LOOP ENDS HERE ---

    closesocket(server_fd);
    WSACleanup();
    return 0;
}
