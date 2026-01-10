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

struct connect_addr_info * create_listener_info(uint16_t private_port, bool use_public_ip);
struct addrinfo * get_listener_info(int type, const char *address, uint16_t port, bool passive);
int get_listener_socket(const char *address, uint16_t port);
int create_client_socket(int listener_sock);
/** @brief Does not close the accept_sock. Caller is responsible for closing the socket. */
const char * receive_data(int accept_sock, int buffer_size);
/** @brief Does not close the accept_sock. Caller is responsible for closing the socket. */
const char * receive_data_from(const char * listener_address, int listener_port, int buffer_size);
int create_poll(void);
ssize_t send_data(int dest, const char *data, size_t size);
const char * make_http_request(const char *type, const char *host, uint16_t port);//TODO: WIP - Only supports GET

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* socketutil_h */
