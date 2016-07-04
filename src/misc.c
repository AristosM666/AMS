#include "misc.h"

#include <string.h>

static long partition(int arr[], int lo, int hi);
static void swap(int *first, int *second);

void quicksort(int arr[], int lo, int hi)
{
  int p;
  if (lo < hi)
  {
    p = partition(&arr[0], lo, hi);
    quicksort(&arr[0], lo, p - 1);
    quicksort(&arr[0], p + 1, hi);
  }
}

static long partition(int arr[], int lo, int hi)
{
  int i, pivotIndex, pivotValue;

  pivotValue = arr[hi];
  pivotIndex = lo;

  for (i = lo; i <= hi - 1; i++)
  {
    if (arr[i] <= pivotValue)
    {
      swap(&arr[pivotIndex], &arr[i]);
      pivotIndex++;
    }
  }

  swap(&arr[pivotIndex], &arr[hi]);
  return pivotIndex;
}

static void swap(int *first, int *second)
{
  int tmp = *first;
  *first = *second;
  *second = tmp;
}

size_t removeDuplicate(char *arr[], size_t nmemb)
{
  char **current, **end = arr + nmemb - 1;
  size_t count = 0;

  for (current = arr + 1; arr < end; arr++, current = arr + 1)
  {
    while (current <= end)
    {
      if (!strcmp(*current, *arr))
      {
        strcpy(*current, *end);
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

boolean freadUntilDelim(FILE* fp, char delim, char* dest)
{
  size_t i = 0;
  char ch;

  while (!feof(fp))
  {
    ch = fgetc(fp);
    if (ch == delim)
    {
      dest[i] = '\0';
      return TRUE;
    }
    dest[i] = ch;
    i++;
  }

  return FALSE;
}

size_t getString(char* dest, size_t size)
{
  size_t len;
	
  fflush(stdin);
  fgets(dest, size, stdin);
  len = strlen(dest);
	
  if (len == size - 1)
    getchar();
	
  return len;
}

boolean isIntBetween(char* src, int min, int max)
{
  size_t i;	
	
  /* Test if all characters are digits */
  for(i = 0; i < strlen(src); i++)
    if (!isdigit(*(src + i)))
      return FALSE;

  /* Test if value is within min/max range */
  if (atoi(src) < min || atoi(src) > max)
    return FALSE;
	
  return TRUE;
}
