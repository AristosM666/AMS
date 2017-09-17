#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#ifndef SIZE_T_MAX
#define SIZE_T_MAX (size_t) -1
#endif

void quickSort (int16_t arr[], intmax_t lo, intmax_t hi);
size_t removeDuplicateStr (char *arr[], size_t nmemb);
size_t removeDuplicateInt (int16_t arr[], size_t nmemb);
bool csvReadValue (FILE *const fp, char *dest);
size_t getString (char *dest, size_t size);
bool isInRange (const char *src, intmax_t min, intmax_t max);
void stripWhiteSpace (char *src);
void pause (void);
void clearConsole (void);
char *strToUpper (char *str);
void flushStdin (void);
__attribute__((noreturn)) void fatal (const char *errMsg);

