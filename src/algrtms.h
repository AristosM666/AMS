#ifndef ALGRTMS_H
#define ALGRTMS_H

#include <stdlib.h> /* included for size_t */
#include <stdio.h> /* included for FILE */

typedef enum { FALSE, TRUE } boolean;

void quicksort(int arr[], int lo, int hi);
size_t removeDuplicate(char *arr[], size_t len);
boolean freadUntilDelim(FILE* fp, char delim, char* dest);

#endif /* ALGRTMS_H */
