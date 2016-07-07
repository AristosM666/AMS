#ifndef MISC_H
#define MISC_H


#include <stdlib.h>  /* included for size_t */
#include <stdio.h>   /* included for FILE   */
#include <stdbool.h> /* included for bool   */


void quicksort(int arr[], int lo, int hi);
size_t removeDuplicate(char *arr[], size_t nmemb);
bool csvReadNextVal(FILE* fp, char* dest);
size_t getString(char* dest, size_t size);
bool isIntBetween(char* src, int min, int max);
void parseWhiteSpace(char* str);
void fatal(char* errMsg);
void clearScreen(void);
void strToUpper(char* str);


#endif /* MISC_H */
