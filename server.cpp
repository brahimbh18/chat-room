#include <csignal>
#include <cstdio>
#include <cstring>
#include <thread>
#include <iostream>
#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in structure 
#include <unistd.h>     // For close function
#include <vector>
#include <algorithm>

using namespace std;

int server_socket;
vector<int> client_sockets;
// char buffer[1024];
void handle_client(int client1, int client2);
void send_to_all_clients(char buffer[1024], int clients_socket);
void handle_signal(int signal);

int main() { 
    signal(SIGINT, handle_signal);

    // Create the socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        return -1;
    }
    
    // Allow reusing tthe address/port
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1) {
        perror("setsockopt failed");
        return -2;
    }

    // Cofigure server address
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    // Bind the socket
    if (bind(server_socket, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        return -3;
    }

    // Start listening
    if (listen(server_socket, 10) == -1) {
        perror("Listen failed");
        return -4;
    }

    cout << "🎧 Server is now listening for connections...\n";

    sockaddr_in client_addr{};
    socklen_t client_size = sizeof(client_addr);

    int client1 = accept(server_socket, (struct sockaddr*) &client_addr, (socklen_t*)&client_size);
    cout << "Client 1 connected.\n";

    int client2 = accept(server_socket, (struct sockaddr*) &client_addr, (socklen_t*)&client_size);
    cout << "Client 2 connected.\n";
    
    thread t1 (handle_client, client1, client2);
    thread t2 (handle_client, client2, client1);

    t1.join();
    t2.join();

    close(server_socket);

    return 0;
}

void handle_client(int client1, int client2) {
    char buffer[1024] = {0};
    while (true)  {
        memset(buffer, 0, sizeof(buffer));
        int valread = recv(client1, buffer, 1024, 0);
        if (valread <= 0) {
            cout << "client disconnected\n";
        }
        // cout << "message : " << buffer;
        send(client2, buffer, 1024, 0);
    }
    close(client1);
    close(client2);
    cout << "🔒 Connection closed with client\n";

}

void send_to_all_clients(char buffer[1024], int client_socket) {
    for (int fd: client_sockets) {
        if (fd == client_socket) continue;
        send(fd, buffer, strlen(buffer), 0);
    }
}

void handle_signal(int signal) {
    cout << "\nShutting down server gracefully...\n";
    close(server_socket);
    exit(0);
}