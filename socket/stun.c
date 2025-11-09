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

#pragma mark -
//STUN
//https://gist.github.com/jyaif/e0db3a680443730c05ca36be26f22c93
int get_public_addr_info(const char *stun_server_ip, uint16_t stun_server_port, uint16_t client_port, struct connect_addr_info **info) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("Failed to create STUN socket");
        fprintf(stderr, "STUN socket error -> %s\n", strerror(errno));
        close(sock);
        return -1;
    }
    
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    //server setup
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(stun_server_port);
    int bin_addr = inet_pton(AF_INET, stun_server_ip, &server_addr.sin_addr);
    
    if (bin_addr <= 0) {
        close(sock);
        fprintf(stderr, "STUN invalid IP error -> %s\n", strerror(errno));
        return -1;
    }
    
    //client setup
    memset(&client_addr, 0, client_len);
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(client_port);
    
    int bd = bind(sock, (struct sockaddr *)&client_addr, client_len);
    if (bd < 0) {
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
    
    ssize_t bytes_sent = sendto(sock, &request, sizeof(request), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
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
    
    for (int i = 0; i < ntohs(response.msg_length); i++) {
        printf("%02X ", response.attributes[i]);
    }
    printf("\n");
    
    uint8_t *attr_ptr = (uint8_t*)response.attributes;
    int offset = 0;
    int total_len = ntohs(response.msg_length);
    
    while (offset + 4 <= total_len) {
        uint16_t attr_type = ntohs(*(uint16_t*)(attr_ptr + offset));
        uint16_t attr_len  = ntohs(*(uint16_t*)(attr_ptr + offset + 2));
        
        if (attr_type == STUN_XOR_MAPPED_ADDRESS) {
            if (attr_len >= 8) {
                
                uint8_t family = attr_ptr[offset + 5];
                
                if (family == STUN_IPV_4) {
                    uint16_t port;
                    memcpy(&port, attr_ptr + offset + 6, 2);
                    port = ntohs(port) ^ (STUN_BIND_REQUEST_COOKIE >> 16);
                    
                    uint8_t x_ip[4];
                    memcpy(x_ip, attr_ptr + offset + 8, 4);
                    
                    uint8_t ip_bytes[4];
                    ip_bytes[0] = x_ip[0] ^ ((STUN_BIND_REQUEST_COOKIE >> 24) & 0xFF);
                    ip_bytes[1] = x_ip[1] ^ ((STUN_BIND_REQUEST_COOKIE >> 16) & 0xFF);
                    ip_bytes[2] = x_ip[2] ^ ((STUN_BIND_REQUEST_COOKIE >> 8) & 0xFF);
                    ip_bytes[3] = x_ip[3] ^ (STUN_BIND_REQUEST_COOKIE & 0xFF);
                    
                    struct in_addr addr;
                    memcpy(&addr.s_addr, ip_bytes, 4);
                    
                    char ip_buff[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &addr, ip_buff, sizeof(ip_buff));
                    
                    (*info)->public_ip = strdup(ip_buff);
                    (*info)->port = port;
                    break;
                }
                else {
                    //TODO: ipv6
                    break;
                }
            }
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
