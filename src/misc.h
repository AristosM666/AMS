#pragma once


#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>


void quicksort (int16_t arr[], intmax_t lo, intmax_t hi);
size_t removeDuplicateStr (char *arr[], size_t nmemb);
size_t removeDuplicate (int16_t arr[], size_t nmemb);
bool csvReadNextVal (FILE *fp, char *dest);
size_t getString (char *dest, size_t size);
bool isIntBetween (char *src, intmax_t min, intmax_t max);
void parseWhiteSpace (char *src);
void fatal (char *errMsg);
void pause (void);
void clearScreen (void);
void strToUpper (char *str);

