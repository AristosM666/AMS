#include "algrtms.h"

#include <string.h>

static long partition (int arr[], int lo, int hi);
static void _swap (int *first, int *second);

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
			_swap(&arr[pivotIndex], &arr[i]);
			pivotIndex++;
		}
	}
	
	_swap(&arr[pivotIndex], &arr[hi]);
	return pivotIndex;
}

static void _swap(int *first, int *second)
{
	int tmp = *first;
	*first = *second;
	*second = tmp;
}

size_t removeDuplicate(char *arr[], size_t len)
{
	char **current, **end = arr + len - 1;
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
	
	return count - 1;
}
