#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#ifndef SIZE_T_MAX
#define SIZE_T_MAX (size_t) -1
#endif

void quicksort (int16_t arr[], intmax_t lo, intmax_t hi);
size_t remove_dup_str (char *arr[], size_t nmemb);
size_t remove_dup_int (int16_t arr[], size_t nmemb);
bool csv_read_next (FILE *const fp, char *dest);
size_t get_string (char *dest, size_t size);
bool is_in_range (const char *src, intmax_t min, intmax_t max);
void strip_white_space (char *src);
void pause (void);
void clear_console (void);
char *str_to_upper (char *str);
void flush_stdin (void);
__attribute__((noreturn)) void fatal (const char *emsg);

