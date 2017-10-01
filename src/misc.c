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
    const int16_t pivot_value = arr[hi];
    intmax_t pivot_index = lo;

    for (intmax_t i = lo; i <= hi - 1; i++)
      {
        if (arr[i] <= pivot_value)
          {
            swap (&arr[pivot_index], &arr[i]);
            pivot_index++;
          }
      }

    swap (&arr[pivot_index], &arr[hi]);
    return pivot_index;
}


static void
swap (int16_t *const lval, int16_t *const rval)
{
    const int16_t tmp = *lval;
    *lval = *rval;
    *rval = tmp;
}


size_t
remove_dup_str (char *arr[], const size_t nmemb)
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
                    strncpy (*current, *end, sizeof (char*));
                end--;
              }
            else
            {
                current++;
            }
          }
        count++;
      }

    return count;
}


size_t
remove_dup_int (int16_t arr[], const size_t nmemb)
{
    int16_t *end = arr + nmemb - 1;
    size_t count = 0;

    for (int16_t *current = arr + 1; arr < end; arr++, current = arr + 1)
      {
        while (current <= end)
          {
            if (*current == *arr)
                *current = *end--;
            else
                current++;
          }
        count++;
      }

    return count;
}


bool
csv_read_next (FILE *const fp, char *dest)
{
    char ch;

    do
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
    while (ch != EOF);

    return false;
}


size_t
get_string (char *const dest, const size_t size)
{
    size_t len = 0;
    char ch;
    
    do
      {
        ch = (char) getchar ();
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
is_in_range (const char *const src, const intmax_t min, const intmax_t max)
{
    for (size_t i = 0; i < strlen (src); i++)
      {
        if (!isdigit ( *(src + i) ))
            return false;
      }

    return !(atol (src) < min || atol (src) > max);
}


void
strip_white_space (char *const str)
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
clear_console (void)
{
    printf ("\033[H\033[2J");
    fflush (stdout);
}


char *
str_to_upper (char *const str)
{
    for (size_t i = 0; i < strlen (str); i++)
        str[i] = (char) toupper (str[i]);
    return str;
}


void flush_stdin (void)
{
    char ch; 
    do 
      { 
        ch = (char) getchar (); 
      } 
    while (ch != '\n' && ch != EOF);
}


__attribute__((noreturn)) void
fatal (const char *const emsg)
{
    clear_console ();
    
    fprintf (stderr, "\n\n\t\tFailed while %s!", emsg);
    fflush (stderr);
    
    perror ("\n\n\t\tERROR");
    fflush (stderr);
    
    pause ();

    clear_console ();
    exit (EXIT_FAILURE);
}

