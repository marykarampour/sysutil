//
//  utility.h
//  
//
//  Created by Maryam Karampour on 2025-11-09.
//

#ifndef utility_h
#define utility_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

#define MINC(a, b) ((a) < (b) ? (a) : (b))

struct connect_addr_info {
    int socket;
    const char *private_ip;
    const char *public_ip;
    uint16_t port;
};

const char * int_to_string(int i, size_t size);
uint8_t * randome_int8_array(size_t size);
void randomize_int8_array(uint8_t *arr, size_t size, int range);
uint16_t combine_16(uint8_t i1, uint8_t i2, bool little_endian);
uint32_t combine_32(uint16_t i1, uint16_t i2, bool little_endian);
void split(uint16_t i, uint8_t **l, uint8_t **m);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* utility_h */
