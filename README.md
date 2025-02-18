# Simple HTTP Server with Multi-threading and REST API

This is a simple, multi-threaded HTTP server project that demonstrates basic functionality like serving static files, logging requests, graceful shutdown, and a simple REST API endpoint. The server supports logging and handles HTTP GET requests to serve files, as well as handle errors such as 404 (not found) and 405 (method not allowed).

## How to Run

1. Place the `index.html` file in the `www/` directory.
2. Compile the C++ code:
    ```bash
    g++ -std=c++11 -o simple_http_server simple_http_server.cpp -pthread
    ```
3. Run the server:
    ```bash
    ./simple_http_server
    ```
4. Open your browser and go to `http://localhost:8080/`.

## Features
- Multi-threading for handling multiple client connections.
- Supports basic GET requests to serve static HTML files.
- Basic logging of all incoming requests.
- Graceful shutdown on SIGINT.
- Simple REST API endpoint (`/api/data`) returning JSON.
