#include "misc.h"

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio_ext.h>


static intmax_t partition (int16_t arr[], intmax_t lo, intmax_t hi);
static void swap (int16_t *lval, int16_t *rval);


void
quicksort (int16_t arr[], intmax_t lo, intmax_t hi)
{
    if (lo < hi)
      {
        intmax_t p = partition (arr, lo, hi);
        quicksort (arr, lo, p - 1);
        quicksort (arr, p + 1, hi);
      }
}


static intmax_t
partition (int16_t arr[], intmax_t lo, intmax_t hi)
{
    int16_t pivotValue = arr[hi];
    intmax_t pivotIndex = lo;

    for (intmax_t i = lo; i <= hi - 1; i++)
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
swap (int16_t *lval, int16_t *rval)
{
    int16_t tmp = *lval;
    *lval = *rval;
    *rval = tmp;
}


size_t
removeDuplicateStr (char *arr[], size_t nmemb)
{
    char **end = arr + nmemb - 1;
    size_t count = 0;

    for (char **current = arr + 1; arr < end; arr++, current = arr + 1)
      {
        while (current <= end)
          {
            if (!strcmp (*current, *arr))
              {
                if (current != end)
                    strncpy (*current, *end, sizeof (current));
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
removeDuplicateInt (int16_t arr[], size_t nmemb)
{
    int16_t *end = arr + nmemb - 1;
    size_t count = 0;

    for (int16_t *current = arr + 1; arr < end; arr++, current = arr + 1)
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
    fflush (stdin);
    fgets (dest, (int) size, stdin);
    size_t len = strlen (dest);

    if (len == size - 1)
        getchar ();

    if (dest[strlen (dest)-1] == '\n')
      {
        dest[strlen (dest)-1] = '\0';
        len--;
      }
    
    __fpurge (stdin);
    return len;
}


bool
isIntBetween (char *src, intmax_t min, intmax_t max)
{
    for (size_t i = 0; i < strlen (src); i++)
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
            for (size_t j = i; j < strlen (str); j++)
                str[j] = str[j+1];
          }
        else 
            i++;
      }
    while (ch != '\0');
}

void
pause (void)
{
    fflush (stdin);
    printf ("\tPress [ Enter ] to continue...");
    getchar ();
}

void
fatal (char *errMsg)
{
    clearScreen ();
    fprintf (stderr, "\n\n\t\tFailed while %s!", errMsg);
    perror ("\n\n\t\tERROR");
    __fpurge(stdout);
    printf ("\n\t");
    pause ();

    clearScreen ();
    exit (EXIT_FAILURE);
}


void
clearScreen (void)
{
    printf ("\033[H\033[2J");
}


void
strToUpper (char *str)
{
    for (size_t i = 0; i < strlen (str); i++)
        str[i] = (char) toupper (str[i]);
}

