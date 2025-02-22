#include <cstring>
#include <iostream>
#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in structure 
#include <unistd.h>     // For close function
#include <arpa/inet.h>
#include <thread>


using namespace std;

void receiveMessage(int socket) {
    char buffer[1024] = {0};
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int valread = recv(socket, buffer, 1024, 0);
        if (valread > 0) {
            std::cout << "Received: " << buffer << std::endl; }
    }
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};


    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        cout << "er";
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cout << "Invalid address / Address not supported\n";
        return -1;
    }

    if (connect(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
        cout << "conn failed";
        return -1;
    }

    thread recvThread(receiveMessage, sock);

    while (true) {
        string message;
        getline(cin, message);
        send(sock, message.c_str(), message.size(), 0);
    }

    recvThread.join();
    close(sock);

    return 0;
}