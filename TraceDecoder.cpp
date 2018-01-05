/******************************************************************************
* Course:	ECE 585
* Term:		Winter 2017
* Project:	Last Level Cache Simulator
* Module:	Trace Decoder
* File:		TraceDecoder.cpp
*****************************************************************************/

#include "stdafx.h"
#include "CacheOp.h"
#include "LRU.h"

/* File read parameters. */
#define ARGUMENT_COUNT_3		3
#define MAX_LINE_SIZE			13
#define MAX_FILENAME_SIZE		50
#define ADDR_POS				2
#define BASE_10					10
#define BASE_16					16

/*
 * Structure typedefs.
 */

typedef struct ins
{
	unsigned char command;
	unsigned int address;
} instruction_t;

typedef struct addr_struct
{
	unsigned int index;
	unsigned int tag;
} addr_info_t;

/*
* Global Variables.
*/
addr_info_t addr_info;

void AsciiToHex(char *data, instruction_t *ins)
{
	if (strlen(data) != 0)
	{
		/* Convert Command from Ascii to Integer. */
		ins->command = (unsigned char)strtol(data, NULL, BASE_10);
		/* Convert Address from Ascii to Integer. */
		/* If address is greater than 32-bit, the address will get truncated. */
		ins->address = (unsigned int)strtoull((data + ADDR_POS), NULL, BASE_16);
	}
#ifdef DEBUG_MODE
	printf("%u 0x%x\n", (ins->command), (ins->address));
#endif /* DEBUG_MODE */
}

void HandleTraceCommand(instruction_t *ins)
{
	/* Make the address cache line size aligned. */
	ins->address = ((ins->address) >> BYTE_BITS) << BYTE_BITS;

	/* Store the index and tag. */
	/* Left shift first to eliminate MSBs and then right shift to eliminate LSBs. */
	addr_info.index = ((ins->address) << (TAG_BITS)) >> (TAG_BITS + BYTE_BITS);
	addr_info.tag = ((ins->address) >> (INDEX_BITS + BYTE_BITS));

	switch (ins->command)
	{
	case CPU_READ_DATA:
		CPUReadData(ins->address, addr_info.index, addr_info.tag);
		break;
	case CPU_WRITE_DATA:
		CPUWriteData(ins->address, addr_info.index, addr_info.tag);
		break;
	case CPU_READ_INS:
		CPUReadInstruction(ins->address, addr_info.index, addr_info.tag);
		break;
	case SNOOPED_INVALIDATE:
		SnoopedInvalidate(ins->address, addr_info.index, addr_info.tag);
		break;
	case SNOOPED_READ:
		SnoopedRead(ins->address, addr_info.index, addr_info.tag);
		break;
	case SNOOPED_WRITE:
		SnoopedWrite();
		break;
	case SNOOPED_READ_MODIFY:
		SnoopedReadModify(ins->address, addr_info.index, addr_info.tag);
		break;
	case CLEAR_AND_RESET:
		ClearCacheAndReset();
		break;
	case PRINT_INFO:
		PrintCacheInfo();
		break;
	default:
		printf("Invalid command\n");
		break;
	}

}

int main(int argc, char *argv[])
{
	FILE *Trace = NULL;;
	char *filename = (char *)malloc(MAX_FILENAME_SIZE), *ascii = (char *)malloc(MAX_LINE_SIZE);
	int status = 0;
	instruction_t instruction;

	/* Collect the arguments. We are looking for a three arguments:
	 * first: cacheapp.exe
	 * second: -f
	 * third: trace file name */
	if (argc == ARGUMENT_COUNT_3)
	{
		for (int i = 0; i < (argc - 1); i++)
		{
			if (*argv[i] == '-')
			{
				switch (*(argv[i]+1))
				{
				case 'f':
					strcpy(filename, argv[i+1]);
					i++;
					break;
				default:
					printf("Wrong argument\n");
					goto HELP;
					break;
				}
			
			}

		}
	}
	else
	{
		HELP:
		printf("Enter correct arguments\n");
		printf("Arguments:\n");
		printf("-f <trace file name>\n");
		printf("For example:\ncacheapp.exe -f trace.txt\n");
		printf("Exiting...\n");
		goto EXIT;
	}

	printf("Implementing Last-level Unified Cache shared with 4 processors\n");
	printf("Total Cache Size in Bytes:	        %u\n", TOTAL_CACHE_SIZE);
	printf("Cache Line Size:			%u\n", CACHE_LINE_SIZE);
	printf("Ways per set:				%u\n", WAYS_PER_SET);
	printf("No. of Sets:				%u\n", NO_OF_SETS);
	printf("Trace File Name:			%s\n", filename);
	/* Initialize the LRU counters. */
	InitializeLRUSeq();

	/* Open the trace file. */
	Trace = fopen(filename,"r");
	if (Trace == NULL)
	{
		printf("Failed to open the file");
		goto EXIT;
	}

	/* Read one line at a time from the trace file. */
	memset(ascii, 0, MAX_LINE_SIZE * sizeof (char));
	while (fgets(ascii, MAX_LINE_SIZE, Trace) != NULL)
	{
		AsciiToHex(ascii, &instruction);
		memset(ascii, 0, MAX_LINE_SIZE * sizeof(char));

		HandleTraceCommand(&instruction);
	}

	printf("Cache misses = %u\n", cache_miss);
	printf("Cache hits = %u\n", cache_hit);
	printf("Cache reads = %u\n", cache_read);
	printf("Cache writes = %u\n", cache_write);
	if ((cache_hit + cache_miss) != 0)
		printf("Cache hit ratio = %04f\n", ((double)cache_hit / ((double)cache_hit + (double)cache_miss)));
	else
		printf("Cache counts cleared. Cache hit ratio cannot be calculated.");

EXIT:
	fclose(Trace);
	free(ascii);
	free(filename);
    return 0;
}





