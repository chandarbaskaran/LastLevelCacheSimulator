/******************************************************************************
* Course:	ECE 585
* Term:		Winter 2017
* Project:	Last Level Cache Simulator
* Module:	Notification Operations
* File:		BusOp.h
*****************************************************************************/

#ifndef BUS_OP
#define BUS_OP


/*
* Used to simulate the reporting of snoop results by other caches.
*/
char GetSnoopResult(unsigned int address);

/*
* Used to simulate a bus operation and to capture the snoop results of last
* level caches of other processors.
*/
void BusOperation(char BusOp, unsigned int Address, char *SnoopResult);

/*
* Used to report the result of our snooping bus operations by other caches.
*/
void PutSnoopResult(unsigned int Address, char SnoopResult);

/*
* Used to simulate communication to our upper level cache.
*/
void MessageToL2Cache(char BusOp, unsigned int Address);
#endif /* BUS_OP */
