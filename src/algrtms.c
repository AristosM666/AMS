#include "algrtms.h"

#include <string.h>

static int partition (int arr[], int lo, int hi);
static void _swap (int *e1, int *e2);

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

static int partition(int arr[], int lo, int hi)
{
	int i, storeIndex, pivotIndex, pivotValue;

	pivotIndex = hi / 2;
	pivotValue = arr[pivotIndex];

	_swap(&arr[pivotIndex], &arr[hi]);
	storeIndex = lo;

	for (i = lo; i <= hi - 1; i++)
	{
		if (arr[i] <= pivotValue)
		{
			_swap(&arr[i], &arr[storeIndex]);
			storeIndex++;
		}
	}
	
	_swap(&arr[storeIndex], &arr[hi]);
	return storeIndex;
}

static void _swap(int *e1, int *e2)
{
	int eTmp = *e1;
	*e1 = *e2;
	*e2 = eTmp;
}

int removeDuplicate(char *arr[], int len)
{
	char **current, **end = arr + len - 1;
	int count = 0;
	
	for (current = arr; arr <= end; ++arr, current++)
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

