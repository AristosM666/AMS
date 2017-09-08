#include "misc.h"

#include <stdlib.h>
#include <ctype.h>
#include <string.h>


static intmax_t partition (int16_t arr[], const intmax_t lo, const intmax_t hi);
static void swap (int16_t *lval, int16_t *rval);


void
quicksort (int16_t arr[], const intmax_t lo, const intmax_t hi)
{
    if (lo < hi)
      {
        const intmax_t p = partition (arr, lo, hi);
        quicksort (arr, lo, p - 1);
        quicksort (arr, p + 1, hi);
      }
}


static intmax_t
partition (int16_t arr[], const intmax_t lo, const intmax_t hi)
{
    const int16_t pivotValue = arr[hi];
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
swap (int16_t *const lval, int16_t *const rval)
{
    const int16_t tmp = *lval;
    *lval = *rval;
    *rval = tmp;
}


size_t
removeDuplicateStr (char *arr[], const size_t nmemb)
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
removeDuplicateInt (int16_t arr[], const size_t nmemb)
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
csvReadNextVal (FILE *const fp, char *dest)
{
    char ch;

    do
      {
        ch = fgetc (fp);
        if (ch == ',')
          {
            *dest = '\0';
            return true;
          }
        *dest = ch;
        dest++;
      }
    while (ch != EOF);

    return false;
}


size_t
getString (char *const dest, const size_t size)
{
    size_t len = 0;
    char ch;
    
    do
      {
        ch = getchar ();
        if (len < size - 1)
          {
            dest[len] = ch;
            len++;
          }
      }
    while (ch != '\n' && ch != EOF);
    
    if (dest[len - 1] == '\n')
        len--;

    dest[len] = '\0';

    return len;
}


bool
isIntBetween (const char *const src, const intmax_t min, const intmax_t max)
{
    for (size_t i = 0; i < strlen (src); i++)
      {
        if (!isdigit ( *(src + i) ))
            return false;
      }

    return !(atol (src) < min || atol (src) > max);
}


void
parseWhiteSpace (char *const str)
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
    printf ("\n\t\tPress [ Enter ] to continue...");
    fflush (stdout);
    
    flush_stdin ();
}


void
clearScreen (void)
{
    printf ("\033[H\033[2J");
    fflush (stdout);
}


char *
strToUpper (char *const str)
{
    for (size_t i = 0; i < strlen (str); i++)
        str[i] = toupper (str[i]);
    return str;
}


void flush_stdin (void)
{
    char ch; 
    do 
      { 
        ch = getchar (); 
      } 
    while (ch != '\n' && ch != EOF);
}


__attribute__((noreturn)) void
fatal (const char *const errMsg)
{
    clearScreen ();
    
    fprintf (stderr, "\n\n\t\tFailed while %s!", errMsg);
    fflush (stderr);
    
    perror ("\n\n\t\tERROR");
    fflush (stderr);
    
    pause ();

    clearScreen ();
    exit (EXIT_FAILURE);
}

