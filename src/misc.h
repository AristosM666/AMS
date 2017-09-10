#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#ifndef SIZE_T_MAX
#define SIZE_T_MAX (size_t) -1
#endif

void quicksort (int16_t arr[], intmax_t lo, intmax_t hi);
size_t removeDuplicateStr (char *arr[], size_t nmemb);
size_t removeDuplicateInt (int16_t arr[], size_t nmemb);
bool csvReadNextVal (FILE *const fp, char *dest);
size_t getString (char *dest, size_t size);
bool isIntBetween (const char *src, intmax_t min, intmax_t max);
void parseWhiteSpace (char *src);
void pause (void);
void clearScreen (void);
char *strToUpper (char *str);
void flush_stdin (void);
__attribute__((noreturn)) void fatal (const char *errMsg);

