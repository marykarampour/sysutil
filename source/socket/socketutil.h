//
//  socketutil.h
//  
//
//  Created by Maryam Karampour on 2025-11-09.
//

#ifndef socketutil_h
#define socketutil_h

#ifdef __cplusplus
extern "C" {
#endif

#include <poll.h>
#include "stun.h"

typedef struct {
    uint8_t *bytes;
    size_t length;
} recv_bytes_t;

struct connect_addr_info * create_listener_info(uint16_t private_port, bool use_public_ip, bool use_ipv6);
struct addrinfo * get_listener_info(int type, const char *address, uint16_t port, bool passive);
int get_listener_socket(const char *address, uint16_t port, bool use_ipv6);
int create_client_socket(int listener_sock);
/** @brief Does not close the accept_sock. Caller is responsible for closing the socket. */
unsigned char * receive_data(int accept_sock, int buffer_size, bool use_ssl);
/** @brief Does not close the accept_sock. Caller is responsible for closing the socket. */
unsigned char * receive_data_from(const char * listener_address, int listener_port, int buffer_size, bool use_ssl);
int create_poll(void);
ssize_t send_data(int dest, const char *data, size_t size, bool use_ssl);

recv_bytes_t receive_bytes(int sock, size_t buffer_size);
recv_bytes_t receive_bytes_from(const char *listener_address, int listener_port, size_t buffer_size);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* socketutil_h */
