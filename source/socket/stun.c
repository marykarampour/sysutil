//
//  stun.c
//
//
//  Created by Maryam Karampour on 2025-11-09.
//

#include "stun.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static uint16_t const STUN_BIND_REQUEST_TYPE    = 0x0001;
static uint16_t const STUN_BIND_RESPONSE_TYPE   = 0x0101;
static uint32_t const STUN_BIND_REQUEST_COOKIE  = 0x2112A442;
static uint16_t const STUN_MAPPED_ADDRESS       = 0x0001;
static uint16_t const STUN_XOR_MAPPED_ADDRESS   = 0x0020;
static uint8_t const STUN_IPV_4                 = 0x01;
static uint8_t const STUN_IPV_6                 = 0x02;

struct stun_request_attributes {
    uint16_t msg_type;
    uint16_t msg_length;
    uint32_t magic_cookie;
    uint8_t transaction_id[12];
};

struct stun_response_attributes {
    uint16_t msg_type;
    uint16_t msg_length;
    uint32_t magic_cookie;
    uint8_t transaction_id[12];
    uint8_t attributes[2000];
};

struct stun_xor_mapped_address {
    uint8_t reserved;
    uint8_t family;
    uint16_t port;
    uint32_t address;// IPv4 - what about IPv6?
};

int init_addr(struct sockaddr_storage *addr, socklen_t *addr_len, int family, const char *ip, uint16_t port) {
    memset(addr, 0, sizeof(*addr));
    if (family == AF_INET6) {
        struct sockaddr_in6 *addr_6 = (struct sockaddr_in6 *)addr;
        *addr_len = sizeof(*addr_6);
        addr_6->sin6_family = AF_INET6;
        addr_6->sin6_port = htons(port);
        if (inet_pton(AF_INET6, ip, &addr_6->sin6_addr) <= 0)
            return -1;
    }
    else {
        struct sockaddr_in *addr_4 = (struct sockaddr_in *)addr;
        *addr_len = sizeof(*addr_4);
        addr_4->sin_family = AF_INET;
        addr_4->sin_port = htons(port);
        if (inet_pton(AF_INET, ip, &addr_4->sin_addr) <= 0)
            return -1;
    }

    return 0;
}

int parse_xor_mapped_address(const uint8_t *attr_ptr, uint16_t attr_len, bool useIPv6, struct stun_request_attributes *request, struct connect_addr_info *info, uint16_t client_port) {
    uint8_t family = attr_ptr[5];
    if ((attr_len >= 20 && family == STUN_IPV_6) || (attr_len >= 8 && family == STUN_IPV_4)) {
        uint16_t port;
        memcpy(&port, attr_ptr + 6, 2);
        port = ntohs(port) ^ (STUN_BIND_REQUEST_COOKIE >> 16);
        
        int size = useIPv6 ? 16 : 4;
        uint8_t x_ip[size];
        memcpy(x_ip, attr_ptr + 8, size);
        
        uint8_t ip_bytes[size];
        
        if (useIPv6) {
            for (int i = 0; i < 16; i++) {
                if (i < 4)
                    ip_bytes[i] = x_ip[i] ^ ((STUN_BIND_REQUEST_COOKIE >> (24 - i*8)) & 0xFF);
                else
                    ip_bytes[i] = x_ip[i] ^ request->transaction_id[i - 4];
            }
            
            struct in6_addr addr6;
            memcpy(&addr6.s6_addr, ip_bytes, 16);
            
            char ip_buff[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &addr6, ip_buff, sizeof(ip_buff));
            
            info->public_ip = strdup(ip_buff);
        }
        else {
            ip_bytes[0] = x_ip[0] ^ ((STUN_BIND_REQUEST_COOKIE >> 24) & 0xFF);
            ip_bytes[1] = x_ip[1] ^ ((STUN_BIND_REQUEST_COOKIE >> 16) & 0xFF);
            ip_bytes[2] = x_ip[2] ^ ((STUN_BIND_REQUEST_COOKIE >> 8) & 0xFF);
            ip_bytes[3] = x_ip[3] ^ (STUN_BIND_REQUEST_COOKIE & 0xFF);
            
            struct in_addr addr;
            memcpy(&addr.s_addr, ip_bytes, 4);
            
            char ip_buff[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &addr, ip_buff, sizeof(ip_buff));
            
            info->public_ip = strdup(ip_buff);
        }
        info->public_port = port;
        info->private_port = client_port;
        return 0;
    }
    return -1;
}

#pragma mark -
//STUN
//https://gist.github.com/jyaif/e0db3a680443730c05ca36be26f22c93
int get_public_addr_info(const char *stun_server_ip, uint16_t stun_server_port, uint16_t client_port, struct connect_addr_info **info, bool useIPv6) {
    int family = useIPv6 ? AF_INET6 : AF_INET;
    int sock = socket(family, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("Failed to create STUN socket");
        fprintf(stderr, "STUN socket error -> %s\n", strerror(errno));
        return -1;
    }

    struct sockaddr_storage server_addr;
    struct sockaddr_storage client_addr;
    struct sockaddr *server_sa = (struct sockaddr *)&server_addr;
    struct sockaddr *client_sa = (struct sockaddr *)&client_addr;
    socklen_t server_len;
    socklen_t client_len;
    const char *ip = useIPv6 ? "::" : "0.0.0.0";

    if (init_addr(&server_addr, &server_len, family, stun_server_ip, stun_server_port) < 0) {
        close(sock);
        fprintf(stderr, "Invalid STUN server IP\n");
        return -1;
    }

    if (init_addr(&client_addr, &client_len, family, ip, client_port) < 0) {
        close(sock);
        fprintf(stderr, "Invalid local bind IP\n");
        return -1;
    }

    if (bind(sock, client_sa, client_len) < 0) {
        close(sock);
        fprintf(stderr, "STUN bind socket error -> %s\n", strerror(errno));
        return -1;
    }

    struct stun_request_attributes request;
    memset(&request, 0, sizeof(request));

    request.msg_type = htons(STUN_BIND_REQUEST_TYPE);
    request.msg_length = htons(0x0000);
    request.magic_cookie = htonl(STUN_BIND_REQUEST_COOKIE);
    randomize_int8_array(request.transaction_id, sizeof(request.transaction_id), 256);

    ssize_t bytes_sent = sendto(sock, &request, sizeof(request), 0, server_sa, server_len);
    if (bytes_sent < 0) {
        perror("Failed to send STUN request");
        fprintf(stderr, "STUN request error -> %s\n", strerror(errno));
        close(sock);
        return -1;
    }

    struct stun_response_attributes response;
    memset(&response, 0, sizeof(response));

    ssize_t bytes_recv = recvfrom(sock, &response, sizeof(response), 0, NULL, 0);
    if (bytes_recv < 0) {
        perror("Failed to receiving STUN response");
        fprintf(stderr, "STUN response error -> %s\n", strerror(errno));
        close(sock);
        return -1;
    }
    
    if (response.msg_type != htons(STUN_BIND_RESPONSE_TYPE)) {
        perror("STUN response type mismatch");
        close(sock);
        return -1;
    }
    
    if (response.magic_cookie != htonl(STUN_BIND_REQUEST_COOKIE)) {
        perror("STUN response cookie mismatch");
        close(sock);
        return -1;
    }
    
    if (memcmp(response.transaction_id, request.transaction_id, sizeof(request.transaction_id))) {
        perror("STUN response id mismatch");
        close(sock);
        return -1;
    }

    uint8_t *attr_ptr = (uint8_t*)response.attributes;
    int offset = 0;
    int total_len = ntohs(response.msg_length);
    
    while (offset + 4 <= total_len) {
        uint16_t attr_type = ntohs(*(uint16_t*)(attr_ptr + offset));
        uint16_t attr_len  = ntohs(*(uint16_t*)(attr_ptr + offset + 2));
        
        if (attr_type == STUN_XOR_MAPPED_ADDRESS) {
            if (parse_xor_mapped_address(attr_ptr + offset, attr_len, useIPv6, &request, *info, client_port) == 0)
                break;
        }

        offset += 4 + ((attr_len + 3) & ~3);
    }

    if ((*info)->public_ip == NULL) {
        perror("STUN response parsing failed");
        close(sock);
        return -1;
    }

    close(sock);
    return 0;
}
