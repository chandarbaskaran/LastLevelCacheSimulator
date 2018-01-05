/******************************************************************************
 * Course:	ECE 585
 * Term:	Winter 2017
 * Project: Last Level Cache Simulator
 * Module:	Cache Operations
 * File:	CacheOp.cpp
 *****************************************************************************/

#include "stdafx.h"
#include "CacheOp.h"
#include "LRU.h"
#include "BusOp.h"

/* Global Variables. */
cache_info_t cache[NO_OF_SETS][WAYS_PER_SET] = { 0 };
unsigned int cache_read = 0, cache_write = 0, cache_hit = 0, cache_miss = 0;

void UpdateCacheLineTag(unsigned int index, unsigned int way, unsigned int tag)
{
	cache[index][way].tag = tag;
}

void UpdateCacheLineState(unsigned int index, unsigned int way, unsigned int state)
{
	cache[index][way].state = state;
}

/* Match tag and return the status. Update 'way' on match. */
bool CheckInL3(unsigned int index, unsigned int tag, unsigned int *way)
{
	bool match_stat = FALSE;

	/* Look through the Cache to match the tag bits. */
	for (int i = 0; i < WAYS_PER_SET; i++)
	{
		if (cache[index][i].state != INVALID)
		{
			if (tag == cache[index][i].tag)
			{
				/* If there is a match, no need to go through entire loop. 
				 * Return from here. */
				match_stat = TRUE;
				*way = i;
				return match_stat;
			}
		}
	}

	return match_stat;
}

void CPUReadData(unsigned int address, unsigned int index, unsigned int tag)
{
	bool match_status = FALSE;
	bool eviction_status = NOT_EVICTED;
	unsigned int way;
	char snoop_result = 0;

	/* Cache Read Event. */
	cache_read++;

	match_status = CheckInL3(index, tag, &way);
	if (match_status == TRUE)
	{
		/* Cache Hit. */
		cache_hit++;

		/* Update LRU. */
		MakeMRU(way, index);
	}
	else
	{
		/* Cache Miss. */
		cache_miss++;

		/* Update LRU and send a message to L2 on evicting a line and perform any bus
		 * operations if required. */
		eviction_status = GetLRU(index, &way);
		if (eviction_status == EVICTED)
		{
			/* Write-back to memory */
			if (cache[index][way].state == MODIFIED)
			{
				BusOperation(WRITE, (((unsigned int)cache[index][way].tag << (INDEX_BITS + BYTE_BITS))|((unsigned int)index << BYTE_BITS)), &snoop_result);
			}
			/* Send a message to L2 */
			MessageToL2Cache(INVALIDATE, (((unsigned int)cache[index][way].tag << (INDEX_BITS + BYTE_BITS)) | ((unsigned int)index << BYTE_BITS)));
		}

		/*Update the Cache line tag. */
		UpdateCacheLineTag(index, way, tag);

		/* Check in other L3 caches to decide the state of the cacheline. */
		BusOperation(READ, address, &snoop_result);

		switch (snoop_result)
		{
		case NOHIT:
			/* Move tag and byte update above switch. */
			UpdateCacheLineState(index, way, EXCLUSIVE);
			break;
		case HIT:
			UpdateCacheLineState(index, way, SHARED);
			break;
		case HITM:
			/* The expectation is that other cache will write the data back to memory. */
			UpdateCacheLineState(index, way, SHARED);
			break;
		}
	}
}

void CPUWriteData(unsigned int address, unsigned int index, unsigned int tag)
{
	bool match_status = FALSE;
	bool eviction_status = NOT_EVICTED;
	unsigned int way;
	char snoop_result = 0;

	/* Cache Write Event. */
	cache_write++;

	match_status = CheckInL3(index, tag, &way);
	if (match_status == TRUE)
	{
		/* Cache Hit. */
		cache_hit++;

		/* Update LRU. */
		MakeMRU(way, index);

		/* Check the state of this line. */
		/* If the state is already 'Modified/Exclusive' no need of any Bus operation. */
		if (cache[index][way].state == SHARED)
		{
			/* If this line is shared, ask other L3 caches to invalidate their copies. 
			 * Bus operation is required only if the current line state is SHARED. */
			BusOperation(INVALIDATE, address, &snoop_result);
		}

		/* Update the state of line. Irrespective of the previous state the line state
		* will be as below. */
		UpdateCacheLineTag(index, way, tag);
		UpdateCacheLineState(index, way, MODIFIED);
	}
	else
	{
		/* Cache Miss. */
		cache_miss++;

		eviction_status = GetLRU(index, &way);
		if (eviction_status == EVICTED)
		{
			if (cache[index][way].state == MODIFIED)
			{
				BusOperation(WRITE, (((unsigned int)cache[index][way].tag << (INDEX_BITS + BYTE_BITS)) | ((unsigned int)index << BYTE_BITS)), &snoop_result);
			}
			MessageToL2Cache(INVALIDATE, (((unsigned int)cache[index][way].tag << (INDEX_BITS + BYTE_BITS)) | ((unsigned int)index << BYTE_BITS)));
		}

		/* Send a bus operation for Read with Intent to Modify. */
		BusOperation(RWIM, address, &snoop_result);

		/* Update the state of line. Irrespective of the previous state the line state
		* will be as below. */
		UpdateCacheLineTag(index, way, tag);
		UpdateCacheLineState(index, way, MODIFIED);
	}
}

void CPUReadInstruction(unsigned int address, unsigned int index, unsigned int tag)
{
	CPUReadData(address, index, tag);
}

void SnoopedInvalidate(unsigned int address, unsigned int index, unsigned int tag)
{
	bool match_status = FALSE;
	unsigned int way;

	/* Check in L3. */
	match_status = CheckInL3(index, tag, &way);
	if (match_status == TRUE)
	{
		/* If the line is in SHARED state only then Invalidate can be snooped.
		* Otherwise it is just an error condition. */
		if (cache[index][way].state == SHARED)
		{
			/* Mark the line as invalid and send a message to L2. */
			UpdateCacheLineState(index, way, INVALID);
			MessageToL2Cache(INVALIDATE, address);
			MakeLRU(way, index);
		}
	}
}

void SnoopedRead(unsigned int address, unsigned int index, unsigned int tag)
{
	bool match_status = FALSE;
	unsigned int way;
	char snoop_result;

	/* Check in L3. */
	match_status = CheckInL3(index, tag, &way);

	if (match_status == TRUE)
	{
		/* Check the current state. */
		switch (cache[index][way].state)
		{
		case MODIFIED:
			PutSnoopResult(address, HITM);
			/* If other L3 cache is trying to read a line which is in Modified state 
			 * in your cache, then write the line back to memory and change the state
			 * to SHARED. */
			BusOperation(WRITE, address, &snoop_result);
			UpdateCacheLineState(index, way, SHARED);
			break;
		case EXCLUSIVE:
			PutSnoopResult(address, HIT);
			UpdateCacheLineState(index, way, SHARED);
			break;
		case SHARED:
			PutSnoopResult(address, HIT);
			break;
		}

	}
	else
	{
		PutSnoopResult(address, NOHIT);
	}
}


void SnoopedWrite()
{
	/* Do nothing. */
}

void SnoopedReadModify(unsigned int address, unsigned int index, unsigned int tag)
{
	bool match_status = FALSE;
	unsigned int way;
	char snoop_result;

	/* Check in L3. */
	match_status = CheckInL3(index, tag, &way);

	if (match_status == TRUE)
	{
		/* Check the current state. */
		switch (cache[index][way].state)
		{
		case MODIFIED:
			PutSnoopResult(address, HITM);
			/* If other L3 cache is trying to read a line which is in Modified state
			* in your cache, then write the line back to memory and invalidate your
			* copy and send a message to L2 to invalidate its copy if present. */
			BusOperation(WRITE, address, &snoop_result);
			break;
		case EXCLUSIVE:
		case SHARED:
			PutSnoopResult(address, HIT);
			break;
		}
		/* Following instructions are common for all above cases. */
		UpdateCacheLineState(index, way, INVALID);
		MessageToL2Cache(INVALIDATE, address);
		MakeLRU(way, index);
	}
	else
	{
		PutSnoopResult(address, NOHIT);
	}
}

void ClearCacheAndReset()
{
	/* Clear all counts. */
	cache_hit = 0;
	cache_miss = 0;
	cache_read = 0;
	cache_write = 0;

	/* Mark all states as invalid. */
	for (int index = 0; index < NO_OF_SETS; index++)
	{
		for (int way = 0; way < WAYS_PER_SET; way++)
		{
			cache[index][way].state = INVALID;
		}
	}

	/* Initialize the LRU counters. */
	InitializeLRUSeq();
}

void PrintCacheInfo()
{
	char state;
	bool all_invalid_flag = TRUE;

	/* Mark all states as invalid. */
	for (int index = 0; index < NO_OF_SETS; index++)
	{
		for (int way = 0; way < WAYS_PER_SET; way++)
		{
			if (cache[index][way].state != INVALID)
			{
				/* If there is atleast one valid line, then set all_invalid_flag to FALSE. */
				if (all_invalid_flag == TRUE)
				{
					/* Print the below line only once. */
					printf("Index     Way   Tag      State    Address         LRU\n");
					all_invalid_flag = FALSE;
				}
				state = (cache[index][way].state == MODIFIED) ? 'M' : (cache[index][way].state == EXCLUSIVE) ? 'E' : 'S';
				printf("%08x  %02d   %08x    %c      %08x          %u\n", index, way, cache[index][way].tag, state, ((cache[index][way].tag << (INDEX_BITS + BYTE_BITS)) | (index << BYTE_BITS)), lru_sequence[index][way]);
			}
		}
	}

	/* all_invalid_flag will be TRUE if there are no valid lines in cache. */
	if (all_invalid_flag == TRUE)
	{
		printf("No valid lines in cache.\n");
	}
}

