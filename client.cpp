#include <cstring>
#include <iostream>
#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in structure 
#include <unistd.h>     // For close function

using namespace std;

int main() {
    // Create a socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket failed");
        return -1;
    }

    // Server address setup
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Connect to the server
    if (connect(client_socket, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        return -1;
    }
    cout << "✅ Connected to server!\n";

    char buffer[1024];

    // Receive the welcome message from the server
    memset(buffer, 0, sizeof(buffer));
    int bytes_received = recv(client_socket, buffer, sizeof(buffer) -1, 0);
    
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0'; 
        cout << "📩 Server: " << buffer << endl;
    }
    

    while (true) {
        cout << "You: ";
        string user_input;
        getline(cin, user_input);

        // Send message to server
        send(client_socket, user_input.c_str(), user_input.size(), 0);

        // Check if the user wants to exit
        if (user_input == "exit") {
            cout << "🔌 Disconnecting from server...\n";
            break;
        }

        // Receive server response
        memset(buffer, 0, sizeof(buffer));
        bytes_received = recv(client_socket, buffer, sizeof(buffer) -1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            cout << "📨 Server: " << buffer << endl;
        } else if (bytes_received == 0) {
            cout << "❌ Server closed the connection\n";
            break;
        } else {
            perror("Receive failed");
            break;
        }
    }

    // Close the socket
    close(client_socket);
    return 0;
}