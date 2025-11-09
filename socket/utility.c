//
//  utility.c
//  
//
//  Created by Maryam Karampour on 2025-11-09.
//

#include "utility.h"

const char * int_to_string(int i, size_t size) {
    char num[size];
    sprintf(num, "%d", i);
    char *str = malloc(sizeof(num));
    strcpy(str, num);
    return str;
}

uint8_t * randome_int8_array(size_t size) {
    uint8_t *arr = malloc(size * sizeof(uint8_t));
    if (arr == NULL) return NULL;
    
    for (int i=0; i<size; i++) {
        arr[i] = rand() % 1024;
    }
    return arr;
}

void randomize_int8_array(uint8_t *arr, size_t size, int range) {
    arr = malloc(size * sizeof(uint8_t));
    if (arr == NULL) return;
    
    for (int i=0; i<size; i++) {
        arr[i] = rand() % range;
    }
}

uint16_t combine_16(uint8_t i1, uint8_t i2, bool little_endian) {
    return little_endian ? (((uint16_t)i2 << 8) | i1) : (((uint16_t)i1 << 8) | i2);
}

uint32_t combine_32(uint16_t i1, uint16_t i2, bool little_endian) {
    return little_endian ? (((uint32_t)i2 << 8) | i1) : (((uint32_t)i1 << 8) | i2);
}

void split(uint16_t i, uint8_t **l, uint8_t **m) {
    if (l != NULL) **l = (uint8_t)(i & 0x00FF);
    if (m != NULL) **m = (uint8_t)(i >> 8);
}
