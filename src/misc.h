#pragma once


#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

#define flush_stdin() char ch; do { ch = getchar (); } while (ch != '\n' && ch != EOF);

void quicksort (int16_t arr[], const intmax_t lo, const intmax_t hi);
size_t removeDuplicateStr (char *arr[], const size_t nmemb);
size_t removeDuplicateInt (int16_t arr[], const size_t nmemb);
bool csvReadNextVal (FILE * const fp, char *dest);
size_t getString (char *dest, const size_t size);
bool isIntBetween (const char *src, const intmax_t min, const intmax_t max);
void parseWhiteSpace (char *src);
void pause (void);
void clearScreen (void);
char *strToUpper (char *str);
__attribute__((noreturn)) void fatal (const char *errMsg);
