# Web Server Project

This is a custom-built web server implemented in **C++** as part of the **42 School curriculum**. The goal of this project is to develop a robust and fully functional HTTP web server from scratch, learning the intricacies of network programming, concurrency, and request handling. The server adheres to the HTTP/1.1 protocol and is capable of handling dynamic web pages, serving static files, and more.

## Table of Contents
- [Project Description](#project-description)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Configuration](#configuration)
- [Authors](#authors)

## Project Description

The **Web Server Project** is a fundamental part of the 42 School curriculum designed to deepen understanding of how HTTP servers work. It challenges students to handle multiple simultaneous client connections, manage file requests, and interpret HTTP requests and responses. The server can be configured with a custom configuration file, allowing for flexibility in routes, error pages, and other server behaviors.

This project provides insight into low-level network programming using **sockets**, **multi-threading** for handling concurrent connections, and **file I/O** operations, building a complete web server that can handle typical client requests, like serving HTML, CSS, JavaScript, and images.

## Features

- **Handles multiple connections**: Uses multi-threading or multiplexing to serve several clients at the same time.
- **Static file serving**: Capable of serving HTML, CSS, JS, images, and other static assets.
- **Custom error pages**: Supports custom error responses like 404 Not Found, 500 Internal Server Error, etc.
- **Route configuration**: Allows setting up different endpoints in a configuration file.
- **CGI handling**: Supports dynamic content generation via CGI scripts.
- **HTTP request parsing**: Correctly handles GET, POST, and DELETE requests.
- **Keep-Alive connection support**: Efficient handling of persistent connections.

## Installation

To install and set up the project locally, follow the instructions below:

1. **Clone the repository**:
   ```bash
   git clone https://github.com/username/webserver.git
   cd webserver
   make

## Usage
./webserver [config_file]

## Configuration
```bash
server {
    listen 8080;
    server_name localhost;

    location / {
        root /www/data;
        index index.html;
    }

    error_page 404 /404.html;
}
```

## Authors

- **Ashley Fletcher** - *Developer* - [GitHub Profile](https://github.com/ashleyfletcher76)
- **Muhammet Köse** - *Developer* - [GitHub Profile](https://github.com/masummmm54)
