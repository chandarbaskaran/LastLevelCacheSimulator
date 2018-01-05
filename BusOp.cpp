/******************************************************************************
* Course:	ECE 585
* Term:		Winter 2017
* Project:	Last Level Cache Simulator
* Module:	Notification Operations
* File:		BusOp.cpp
*****************************************************************************/
#include "stdafx.h"
#include "CacheOp.h"
/*
* Used to simulate the reporting of snoop results by other caches.
*/
char GetSnoopResult(unsigned int address)
{
	char snoop_res = NOHIT;
	unsigned int snoop_param;

	/* Sample bit 0 and bit 1 of Tag and assign to snoop_param.
	* snoop_param = 0 => HIT
	* snoop_param = 1 => HITM
	* snoop_param = 2,3 => NOHIT */
	snoop_param = (address >> (INDEX_BITS + BYTE_BITS) & 0x03);
	if (snoop_param == 0)
		snoop_res = HIT;
	else if (snoop_param == 1)
		snoop_res = HITM;
	else
		snoop_res = NOHIT;

	return snoop_res;
}

/*
* Used to simulate a bus operation and to capture the snoop results of last
* level caches of other processors.
*/
void BusOperation(char BusOp, unsigned int Address, char *SnoopResult)
{
	*SnoopResult = GetSnoopResult(Address);
#ifndef SILENT
	printf("BusOp: %u, Address : %x, Snoop Result : %u\n", BusOp, Address, *SnoopResult);
#endif
}


/*
* Used to report the result of our snooping bus operations by other caches.
*/
void PutSnoopResult(unsigned int Address, char SnoopResult)
{
#ifndef SILENT
	printf("SnoopResult: Address %x, SnoopResult : %u\n", Address, SnoopResult);
#endif
}

/*
* Used to simulate communication to our upper level cache.
*/
void MessageToL2Cache(char BusOp, unsigned int Address)
{
#ifndef SILENT
	printf("L2: %u %x\n", BusOp, Address);
#endif
}