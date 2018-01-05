/******************************************************************************
* Course:	ECE 585
* Term:		Winter 2017
* Project:	Last Level Cache Simulator
* Module:	LRU Handler
* File:		LRU.cpp
*****************************************************************************/

#include "stdafx.h"
#include "CacheOp.h"
#include "LRU.h"

/*Global Variables. */
unsigned int lru_sequence[NO_OF_SETS][WAYS_PER_SET] = {0};

/* Print LRU counter values for debug. */
void PrintLRU(unsigned int index)
{
	printf("LRU:%u\n", index);
	for (int way = 0; way < WAYS_PER_SET; way++)
	{
		printf("%u ",lru_sequence[index][way]);
	}
	printf("\n");
}

/* Initialize the sequence of counters. */
void InitializeLRUSeq()
{
	/* Init the counters for ways. */
	for (int index = 0; index < NO_OF_SETS; index++)
	{
		for (int way = 0; way < WAYS_PER_SET; way++)
		{
			lru_sequence[index][way] = way;
		}
	}
}

void MakeMRU(unsigned int way, unsigned int index)
{
	unsigned int temp = lru_sequence[index][way];

	for (int j = 0; j < WAYS_PER_SET; j++) 
	{
		if (lru_sequence[index][j] < temp)
			lru_sequence[index][j]++;
	}

	lru_sequence[index][way] = 0;
}

void MakeLRU(unsigned int way, unsigned int index)
{
	unsigned int temp = lru_sequence[index][way];

	for (int j = 0; j < WAYS_PER_SET; j++)
	{
		if (lru_sequence[index][j] > temp)
			lru_sequence[index][j]--;
	}

	lru_sequence[index][way] = WAYS_PER_SET - 1;
}

bool GetLRU(unsigned int index, unsigned int *way)
{
	bool eviction_stat = EVICTED;

	/* Check for the way with highest count */
	for (int i = 0; i < WAYS_PER_SET; i++)
	{
		if (lru_sequence[index][i] == (WAYS_PER_SET - 1))
			*way = i;
	}
	
	/* Mark it as most recently used. */
	MakeMRU(*way, index);

	/* If this line is currently invalid, then there is no eviction. */
	if (cache[index][*way].state == INVALID)
	{
		eviction_stat = NOT_EVICTED;
	}


	return eviction_stat;
}