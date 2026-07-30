#ifndef STORAGE_INI_H
#define STORAGE_INI_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

bool storage_ini_read_string(
    FILE *fp,
    const char *key,
    char *value,
    size_t value_size);

bool storage_ini_read_float(
    FILE *fp,
    const char *key,
    float *value);

bool storage_ini_read_uint32(
    FILE *fp,
    const char *key,
    uint32_t *value);

bool storage_ini_write_string(
    FILE *fp,
    const char *key,
    const char *value);

bool storage_ini_write_float(
    FILE *fp,
    const char *key,
    float value);

bool storage_ini_write_uint32(
    FILE *fp,
    const char *key,
    uint32_t value);

#endif