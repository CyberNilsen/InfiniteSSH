#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 2222
#define BUFFER_SIZE 1024

void log_attempt(const char* ip, const char* data) {
    time_t now;
    time(&now);
    char* time_str = ctime(&now);

    time_str[strlen(time_str) - 1] = '\0';
    
    printf("[%s] Connection from %s: %s\n", time_str, ip, data);
    
    FILE *logfile = fopen("honeypot.log", "a");
    if (logfile) {
        fprintf(logfile, "[%s] %s: %s\n", time_str, ip, data);
        fclose(logfile);
    }
}

void handle_client(SOCKET client_socket, struct sockaddr_in client_addr) {
    char buffer[BUFFER_SIZE];
    char client_ip[INET_ADDRSTRLEN];
    
    inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
    
    const char* banner = "SSH-2.0-OpenSSH_8.0\r\n";
    send(client_socket, banner, (int)strlen(banner), 0);
    
    log_attempt(client_ip, "Connected - sent SSH banner");
    
    int loop_count = 0;
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        
        if (bytes_received <= 0) {
            log_attempt(client_ip, "Disconnected");
            break;
        }
        
        buffer[bytes_received] = '\0';
        
        for (int i = 0; i < bytes_received; i++) {
            if (buffer[i] == '\r' || buffer[i] == '\n') {
                buffer[i] = ' ';
            }
        }
        
        log_attempt(client_ip, buffer);
        
        loop_count++;
        
        if (loop_count % 3 == 0) {
            const char* auth_prompt = "Password: ";
            send(client_socket, auth_prompt, (int)strlen(auth_prompt), 0);
        } else if (loop_count % 5 == 0) {

            const char* error = "Authentication failed. Try again.\r\n";
            send(client_socket, error, (int)strlen(error), 0);
        } else {

            const char* responses[] = {
                "debug1: Reading configuration data\r\n",
                "debug1: Connecting to server\r\n",
                "debug1: Connection established\r\n",
                "debug1: Key exchange in progress\r\n"
            };
            int rand_idx = rand() % 4;
            send(client_socket, responses[rand_idx], (int)strlen(responses[rand_idx]), 0);
        }
        
        Sleep(2000); 
        
        if (loop_count > 50) {
            const char* timeout_msg = "Connection timeout.\r\n";
            send(client_socket, timeout_msg, (int)strlen(timeout_msg), 0);
            log_attempt(client_ip, "Forced disconnect after 50 interactions");
            break;
        }
    }
    
    closesocket(client_socket);
}

int main() {
    WSADATA wsaData;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int client_len = sizeof(client_addr);
    
    printf("InfiniteSSH Honeypot starting on port %d...\n", PORT);
    
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return 1;
    }
    
    server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket == INVALID_SOCKET) {
        printf("Socket creation failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed with error: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }
    
    if (listen(server_socket, 5) == SOCKET_ERROR) {
        printf("Listen failed with error: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }
    
    printf("Honeypot listening... Press Ctrl+C to stop.\n");
    
    while (1) {
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket == INVALID_SOCKET) {
            printf("Accept failed with error: %d\n", WSAGetLastError());
            continue;
        }
        
      
        handle_client(client_socket, client_addr);
    }
    
    closesocket(server_socket);
    WSACleanup();
    return 0;
}