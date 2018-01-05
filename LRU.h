/******************************************************************************
* Course:	ECE 585
* Term:		Winter 2017
* Project:	Last Level Cache Simulator
* Module:	LRU Handler
* File:		LRU.h
*****************************************************************************/

#ifndef LRU_HEADER
#define LRU_HEADER

extern unsigned int lru_sequence[NO_OF_SETS][WAYS_PER_SET];

/* Mark a cache line as LRU. */
void MakeLRU(unsigned int way, unsigned int index);

/* Mark a cache line as MRU. */
void MakeMRU(unsigned int way, unsigned int index);

/* Returns the LRU way. */
bool GetLRU(unsigned int index, unsigned int *way);

/* Initialize the sequence of counters. */
void InitializeLRUSeq();

#endif /* LRU_HEADER */