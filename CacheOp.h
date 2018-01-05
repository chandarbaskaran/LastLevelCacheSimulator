/******************************************************************************
* Course:	ECE 585
* Term:		Winter 2017
* Project:	Last Level Cache Simulator
* Module:	Cache Operations
* File:		CacheOp.h
*****************************************************************************/

#ifndef CACHE_OP_HEADER
#define CACHE_OP_HEADER

/* Uncomment the line below to run in debug mode. Configuration for debug mode:
 * 4 ways, 2 sets and 2 byte set size. */
//#define DEBUG_MODE

/* Uncomment the line below to disable all the debug prints. */
//#define SILENT

/*
* Cache Parameters.
*/
#ifndef DEBUG_MODE
#define	ADDRESS_BUS_WIDTH	32					/* 32-bit address */
#define TOTAL_CACHE_SIZE	(16 * 1024 * 1024)	/* 16MB */
#define CACHE_LINE_SIZE		64					/* 64B */
#define WAYS_PER_SET		8					/* 8-way associative */
#define NO_OF_SETS			(TOTAL_CACHE_SIZE/CACHE_LINE_SIZE/WAYS_PER_SET)

#define BYTE_BITS			(unsigned int)log2(CACHE_LINE_SIZE)
#define INDEX_BITS			(unsigned int)log2(NO_OF_SETS)
#define	TAG_BITS			(ADDRESS_BUS_WIDTH - BYTE_BITS - INDEX_BITS)

#else
#define	ADDRESS_BUS_WIDTH	32					/* 32-bit address */
#define TOTAL_CACHE_SIZE	(64*4*2)			
#define CACHE_LINE_SIZE		64					/* 4B */
#define WAYS_PER_SET		4					/* 4-way associative */
#define NO_OF_SETS			(TOTAL_CACHE_SIZE/CACHE_LINE_SIZE/WAYS_PER_SET)

#define BYTE_BITS			(unsigned int)log2(CACHE_LINE_SIZE)					/* TODO:Parametrize this:log(64) to base 2 */
#define INDEX_BITS			(unsigned int)log2(NO_OF_SETS)
#define	TAG_BITS			(ADDRESS_BUS_WIDTH - BYTE_BITS - INDEX_BITS)
#endif // DEBUG_MODE



/*
 * Bus Operation Types. 
 */
#define READ				1 /* Bus Read */
#define WRITE				2 /* Bus Write */
#define INVALIDATE			3 /* Bus Invalidate */
#define RWIM				4 /* Bus Read With Intent to Modify */

 /*
  * Snoop Result types.
  */
#define NOHIT				0 /* No hit */
#define HIT					1 /* Hit */
#define HITM				2 /* Hit to modified line */

/*
 * MESI States.
 */
#define INVALID				0
#define MODIFIED			1
#define EXCLUSIVE			2
#define SHARED				3


/*
 * Trace commands.
 */
#define CPU_READ_DATA		0 /* read request from L1 data cache. */
#define CPU_WRITE_DATA		1 /* write request from L1 data cache. */
#define CPU_READ_INS		2 /* read request from L1 instruction cache */
#define SNOOPED_INVALIDATE	3 /* snooped invalidate command. */ 
#define SNOOPED_READ		4 /* snooped read request. */
#define SNOOPED_WRITE		5 /* snooped write request. */
#define SNOOPED_READ_MODIFY	6 /* snooped read with intent to modify. */
#define CLEAR_AND_RESET		8 /* clear the cache and reset all state. */
#define PRINT_INFO			9 /* print contents and state of each valid cache line. */

#define TRUE				1
#define FALSE				0

#define VALID				1
#define NOT_VALID			0

#define DIRTY				1
#define NOT_DIRTY			0

#define EVICTED				1
#define NOT_EVICTED			0

/*
 * Stuctures.
 */
typedef struct info
{
	unsigned int tag;
	unsigned int state : 2;
}cache_info_t;

/* Global Variables. */
extern cache_info_t cache[NO_OF_SETS][WAYS_PER_SET];
extern unsigned int cache_read, cache_write, cache_hit, cache_miss;

/*
 * Function prototypes. 
 */
void CPUReadData(unsigned int address, unsigned int index, unsigned int tag);

void CPUWriteData(unsigned int address, unsigned int index, unsigned int tag);

void CPUReadInstruction(unsigned int address, unsigned int index, unsigned int tag);

void SnoopedInvalidate(unsigned int address, unsigned int index, unsigned int tag);

void SnoopedRead(unsigned int address, unsigned int index, unsigned int tag);

void SnoopedWrite();

void SnoopedReadModify(unsigned int address, unsigned int index, unsigned int tag);

void ClearCacheAndReset();

void PrintCacheInfo();

char GetSnoopResult(unsigned int Address);

void BusOperation(char BusOp, unsigned int Address, char *SnoopResult);

void PutSnoopResult(unsigned int Address, char SnoopResult);

void MessageToL2Cache(char BusOp, unsigned int Address);

#endif /* CACHE_OP_HEADER */