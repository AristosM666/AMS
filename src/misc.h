#ifndef MISC_H
#define MISC_H

#include <stdlib.h> /* included for size_t */
#include <stdio.h> /* included for FILE */

typedef enum { FALSE, TRUE } boolean;

void quicksort(int arr[], int lo, int hi);
size_t removeDuplicate(char *arr[], size_t nmemb);
boolean freadUntilDelim(FILE* fp, char delim, char* dest);
size_t getString(char* dest, size_t size);
boolean isIntBetween(char* src, int min, int max);

#endif /* MISC_H */
