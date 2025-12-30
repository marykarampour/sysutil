//
//  socketutil.c
//
//
//  Created by Maryam Karampour on 2025-11-09.
//

#include "socketutil.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

struct connect_addr_info * create_listener_info(uint16_t private_port, bool use_public_ip, bool use_ipv6) {
    
    struct connect_addr_info *stun_info = (struct connect_addr_info *)malloc(sizeof(struct connect_addr_info));
    if (use_public_ip) {
        const char *stun_server_ip = use_ipv6 ? "2001:4860:4864:5:8000::1" : "74.125.250.129";
        int result = get_public_addr_info(stun_server_ip, 19302, private_port, &stun_info, use_ipv6);
        if (result < 0) {
            fprintf(stderr, "Failed to get listener address\n");
        }
    }
    
    struct addrinfo *bind_info;
    int sock = -1;
    int yes = 1;
    int family = use_ipv6 ? AF_INET6 : AF_INET;
    struct addrinfo *private_addr = get_listener_info(family, NULL, private_port, true);
    if (private_addr == NULL) {
        return NULL;
    }
    
    for (bind_info = private_addr; bind_info != NULL; bind_info = bind_info->ai_next) {
        sock = socket(bind_info->ai_family, bind_info->ai_socktype, bind_info->ai_protocol);
        if (sock < 0) continue;
        
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
        int bd = bind(sock, bind_info->ai_addr, bind_info->ai_addrlen);
        if (bd < 0) {
            close(sock);
            continue;
        }
        
        break;
    }
    
    if (bind_info == NULL) {
        fprintf(stderr, "Failed to bind listener address -> %s\n", gai_strerror(errno));
        return NULL;
    }
    
    char IP[INET6_ADDRSTRLEN];
    void *addr_ptr = NULL;
    
    if (bind_info->ai_family == AF_INET)
        addr_ptr = &((struct sockaddr_in *)bind_info->ai_addr)->sin_addr;
    else if (bind_info->ai_family == AF_INET6)
        addr_ptr = &((struct sockaddr_in6 *)bind_info->ai_addr)->sin6_addr;

    const char *private_ip = inet_ntop(bind_info->ai_family, addr_ptr, IP, sizeof(IP));
    struct connect_addr_info *info = malloc(sizeof(struct connect_addr_info));
    info->socket = sock;
    info->private_port = stun_info->private_port;
    info->public_port = stun_info->public_port;
    info->private_ip = private_ip;
    info->public_ip = stun_info->public_ip;
    
    freeaddrinfo(private_addr);

    printf("Created new listener\n");
    if (info->public_ip)
        printf("Public ip: %s\n", info->public_ip);
    if (info->private_ip)
        printf("Private ip: %s\n", info->private_ip);
    printf("Socket %d\n", info->socket);
    
    return info;
}

struct addrinfo * get_listener_info(int type, const char *address, uint16_t port, bool passive) {
    
    struct addrinfo hints, *addr_info;
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = type;
    hints.ai_socktype = SOCK_STREAM;
    if (passive) hints.ai_flags = AI_PASSIVE;
    
    const char *p_str = int_to_string(port, 16);
    int addr = getaddrinfo(address, p_str, &hints, &addr_info);
    if (addr != 0 ) {
        fprintf(stderr, "Failed to get listener address -> %s\n", gai_strerror(addr));
        return NULL;
    }
    return addr_info;
}

int get_listener_socket(const char *address, uint16_t port, bool use_ipv6) {
    int family = use_ipv6 ? AF_INET6 : AF_INET;
    struct addrinfo *conn_info;
    struct addrinfo *addr_info = get_listener_info(family, address, port, true);
    int sock = -1;
    
    for (conn_info = addr_info; conn_info != NULL; conn_info = conn_info->ai_next) {
        sock = socket(conn_info->ai_family, conn_info->ai_socktype, conn_info->ai_protocol);
        if (sock < 0) continue;
        
        int conn = connect(sock, conn_info->ai_addr, conn_info->ai_addrlen);
        if (conn < 0) {
            close(sock);
            continue;
        }
        
        break;
    }
    
    if (conn_info == NULL) {
        fprintf(stderr, "Failed to connect to listener at address %s with error -> %s\n", address, strerror(errno));
        return -1;
    }
    
    freeaddrinfo(addr_info);
    return sock;
}

int create_client_socket(int listener_sock) {
    
    struct sockaddr_storage addr_info;
    socklen_t addr_len = sizeof(addr_info);
    
    int sock = accept(listener_sock, (struct sockaddr *)&addr_info, &addr_len);
    
    if (sock == -1) {
        perror("Failed to create accept socket");
        return -1;
    }
    
    return sock;
}

const char * receive_data(int accept_sock, int buffer_size) {
    if (accept_sock == -1 || buffer_size <= 0) return "";

    char *buffer = (char *)malloc(buffer_size * sizeof(char));
    memset(buffer, 0, buffer_size);
    size_t total = 0;

    while (1) {
        ssize_t bytes = recv(accept_sock, buffer + total, buffer_size - total, 0);
        if (bytes < 0) {
            perror("Failed to receive data");
            break;
        }
        if (bytes == 0) {
            printf("Connection %d closed\n", accept_sock);
            break;
        }
        total += bytes;
        //TODO: grow buffer dynamically?
    }

    buffer[total] = '\0';
    close(accept_sock);
    return buffer;
}

recv_bytes_t receive_bytes(int accept_sock, size_t buffer_size) {
    recv_bytes_t result;
    result.bytes = NULL;
    result.length = 0;
    
    if (accept_sock == -1 || buffer_size == 0) return result;
    
    uint8_t *buffer = (uint8_t *)malloc(buffer_size * sizeof(uint8_t));
    memset(buffer, 0, buffer_size);
    size_t total = 0;

    //TODO: Make generic with receive_data?
    while (1) {
        ssize_t bytes = recv(accept_sock, buffer + total, buffer_size - total, 0);
        if (bytes < 0) {
            perror("Failed to receive data");
            break;
        }
        if (bytes == 0) {
            printf("Connection %d closed\n", accept_sock);
            break;
        }
        total += bytes;
        //TODO: grow buffer dynamically?
    }

    result.bytes = buffer;
    result.length = total;
    close(accept_sock);
    return result;
}

const char * receive_data_from(const char * listener_address, int listener_port, int buffer_size) {
    int sock = get_listener_socket(listener_address, listener_port, true);
    return receive_data(sock, buffer_size);
}

recv_bytes_t receive_bytes_from(const char *listener_address, int listener_port, size_t buffer_size) {
    recv_bytes_t empty = {0};
    int sock = get_listener_socket(listener_address, listener_port, true);
    if (sock < 0) return empty;
    return receive_bytes(sock, buffer_size);
}

int create_poll(void) {
    return 0;
}

ssize_t send_data(int dest, const char *data, size_t size) {
    ssize_t se = send(dest, data, size, 0);
    if (se == -1)
        fprintf(stderr, "Failed to send data -> %s\n", strerror(errno));
    return se;
}

//TODO: OpenSSL
const char * make_http_request(const char *type, const char *host, uint16_t port) {
    
    char response[8192];
    const char *format = "%s / HTTP/1.1\r\nHost: %s\r\n";
    size_t len = strlen(format) + strlen(type) + strlen(host);
    char msg[len];
    
    sprintf(msg, format, type, host);
    
    int sock = get_listener_socket(host, port, true);
    if (sock == -1 ) {
        fprintf(stderr, "http request:\n%s\n failed with error -> %s\n", msg, strerror(errno));
        return NULL;
    }
    
    memset(response, 0, sizeof(response));
    send(sock, msg, sizeof(msg), 0);
    int response_size = sizeof(response);
    ssize_t s = 0;
    
    while (0 < (s = recv(sock, response+s, response_size, 0))) {
        response_size -= s;
    }
    
    char *str = malloc(response_size+1);
    strcpy(str, response);
    printf("http response ->\n%s", str);
    close(sock);
    
    return str;
}


#pragma mark -


