#include "misc.h"

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>


static ssize_t partition (int arr[], ssize_t lo, ssize_t hi);
static void swap (int *first, int *second);


void
quicksort (int arr[], ssize_t lo, ssize_t hi)
{
    ssize_t p;
    if (lo < hi)
      {
        p = partition (&arr[0], lo, hi);
        quicksort (&arr[0], lo, p - 1);
        quicksort (&arr[0], p + 1, hi);
      }
}


static ssize_t
partition (int arr[], ssize_t lo, ssize_t hi)
{
    ssize_t i, pivotIndex;
    int pivotValue;

    pivotValue = arr[hi];
    pivotIndex = lo;

    for (i = lo; i <= hi - 1; i++)
      {
        if (arr[i] <= pivotValue)
          {
            swap (&arr[pivotIndex], &arr[i]);
            pivotIndex++;
          }
      }

    swap (&arr[pivotIndex], &arr[hi]);
    return pivotIndex;
}


static void
swap (int *first, int *second)
{
    int tmp = *first;
    *first = *second;
    *second = tmp;
}


size_t
removeDuplicateStr (char *arr[], size_t nmemb)
{
    char **current, **end = arr + nmemb - 1;
    size_t count = 0;

    for (current = arr + 1; arr < end; arr++, current = arr + 1)
      {
        while (current <= end)
          {
            if (!strcmp (*current, *arr))
              {
                (void) strcpy (*current, *end);
                end--;
              }
            else
                current++;
          }
        count++;
      }

    return count;
}


size_t
removeDuplicate (int arr[], size_t nmemb)
{
    int *current, *end = arr + nmemb - 1;
    size_t count = 0;

    for (current = arr + 1; arr < end; arr++, current = arr + 1)
      {
        while (current <= end)
          {
            if (*current == *arr)
              {
                *current = *end;
                end--;
              }
            else
                current++;
          }
        count++;
      }

    return count;
}


bool
csvReadNextVal (FILE *fp, char *dest)
{
    char ch;

    while (!feof (fp))
      {
        ch = (char) fgetc (fp);
        if (ch == ',')
          {
            *dest = '\0';
            return true;
          }
        *dest = ch;
        dest++;
      }

    return false;
}


size_t
getString (char *dest, size_t size)
{
    size_t len;
    char ch;

    (void) fflush (stdin);
    (void) fgets (dest, (int) size, stdin);
    len = strlen (dest);

    if (len == size - 1)
        (void) getchar ();

    if (dest[strlen (dest)-1] == '\n')
      {
        dest[strlen (dest)-1] = '\0';
        len--;
      }
    
    __fpurge (stdin);
    return len;
}


bool
isIntBetween (char *src, ssize_t min, ssize_t max)
{
    size_t i;

    for(i = 0; i < strlen (src); i++)
      {
        if (!isdigit ( *(src + i) ))
            return false;
      }

    return !(atol (src) < min || atol (src) > max);
}


void
parseWhiteSpace (char *str)
{
    size_t i = 0;
    char ch;

    do
      {
        ch = str[i];
        if (ch == ' ' || ch == '\t' || ch == '\n')
          {
            size_t j;
            for (j = i; j < strlen (str); j++)
                str[j] = str[j+1];
          }
        else
            i++;
      }
    while (ch != '\0');
}


void
fatal (char *errMsg)
{
    (void) fprintf (stderr, "\n\n\t\tFailed while %s!", errMsg);
    perror ("\n\t\tERROR");
    (void) printf ("\n\t\tPress [Enter] to Exit..");
    (void) getchar ();

    clearScreen ();
    exit (EXIT_FAILURE);
}


void
clearScreen (void)
{
    (void) printf ("\033[H\033[2J");
}


void
strToUpper (char *str)
{
    size_t i;
    for (i = 0; i < strlen (str); i++)
        str[i] = (char) toupper (str[i]);
}

