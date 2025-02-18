#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <string>
#include <netinet/in.h>
#include <unistd.h>
#include <csignal>

#define PORT 8080
#define MAX_BUFFER 1024
#define ROOT_DIR "./www"

volatile sig_atomic_t keep_running = 1;

void signal_handler(int signum) {
    keep_running = 0;
    std::cout << "Shutting down server..." << std::endl;
}

void send_response(int client_socket, const std::string& status_code, const std::string& content_type, const std::string& body) {
    std::stringstream response;
    response << "HTTP/1.1 " << status_code << "\r\n";
    response << "Content-Type: " << content_type << "\r\n";
    response << "Content-Length: " << body.length() << "\r\n";
    response << "Connection: close\r\n\r\n";
    response << body;

    send(client_socket, response.str().c_str(), response.str().length(), 0);
}

std::string serve_static_file(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void log_request(const std::string& request, const std::string& status_code) {
    std::ofstream log_file("server.log", std::ios::app);
    if (log_file.is_open()) {
        log_file << "Request: " << request << " | Status: " << status_code << "\n";
        log_file.close();
    }
}

void handle_client(int client_socket) {
    char buffer[MAX_BUFFER] = {0};
    int bytes_read = read(client_socket, buffer, MAX_BUFFER);

    if (bytes_read < 0) {
        std::cerr << "Error reading from socket" << std::endl;
        close(client_socket);
        return;
    }

    std::string request(buffer);
    std::cout << "Request: " << request << std::endl;
    log_request(request, "200 OK");

    std::string method, path;
    std::istringstream request_stream(request);
    request_stream >> method >> path;

    if (method == "GET") {
        if (path == "/") {
            path = "/index.html";
        }
        
        std::string full_path = ROOT_DIR + path;
        std::string file_content = serve_static_file(full_path);

        if (file_content.empty()) {
            send_response(client_socket, "404 Not Found", "text/html", "<h1>404 Not Found</h1>");
        } else {
            send_response(client_socket, "200 OK", "text/html", file_content);
        }
    } else {
        send_response(client_socket, "405 Method Not Allowed", "text/html", "<h1>405 Method Not Allowed</h1>");
    }

    close(client_socket);
}

void start_server() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << PORT << "..." << std::endl;

    while (keep_running) {
        client_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }

        std::thread client_thread(handle_client, client_socket);
        client_thread.detach();
    }

    close(server_fd);
}

int main() {
    signal(SIGINT, signal_handler);  
    start_server(); 
    return 0;
}
