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
char buffer[1024];
void handle_client(int client_socket);
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

    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_size = sizeof(client_addr);

        // Accept new client connection
        int client_socket = accept(server_socket, (struct sockaddr*) &client_addr, &client_size);
        if (client_socket == -1) {
            perror("Accept failed");
            continue; // Keep listening for new clients
        }

        client_sockets.push_back(client_socket);
        thread client(handle_client, client_socket);
        client.detach();

        // Close the server
    }
    close(server_socket);

    return 0;
}

void handle_client(int client_socket) {
    cout << "✅ Client connected!\n";

    // Send welcome message
    const char* message = "Welcome to the server!\n";
    send(client_socket, message, strlen(message), 0);
    cout << "📩 Sent message to client: " << message << endl;

    // Handle communication with the client
    while (true) {
        memset(buffer, 0, sizeof(buffer));

        // Receive data
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0'; // Ensure null-terminated string
            cout << "->";
            send_to_all_clients(buffer, client_socket);
            // send(client_socket, buffer, strlen(buffer), 0);
        } else if (bytes_received == 0) {
            client_sockets.erase(std::remove(client_sockets.begin(), client_sockets.end(), client_socket), client_sockets.end());
            cout << "❌ Client disconnected\n";
            break;
        } else {
            perror("Receive failed");
            break;
        }
    }

    // Close the client socket
    close(client_socket);
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