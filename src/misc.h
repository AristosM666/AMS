#ifndef MISC_H
#define MISC_H


#include <stdio.h> 
#include <stdbool.h>
#include <sys/types.h>


void quicksort (int arr[], ssize_t lo, ssize_t hi);
size_t removeDuplicateStr (char *arr[], size_t nmemb);
size_t removeDuplicate (int arr[], size_t nmemb);
bool csvReadNextVal (FILE *fp, char *dest);
size_t getString (char *dest, size_t size);
bool isIntBetween (char *src, ssize_t min, ssize_t max);
void parseWhiteSpace (char *src);
void fatal (char *errMsg);
void clearScreen (void);
void strToUpper (char *str);


#endif /* MISC_H */

