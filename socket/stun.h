//
//  stun.h
//  
//
//  Created by Maryam Karampour on 2025-11-09.
//

#ifndef stun_h
#define stun_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include "utility.h"

/** @brief STUN
 Binding Request and Binding Response.
 Shared Secret Request and Shared Secret Response **/

int get_public_addr_info(const char *stun_server_ip, uint16_t stun_server_port, uint16_t client_port, struct connect_addr_info **info);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* stun_h */
