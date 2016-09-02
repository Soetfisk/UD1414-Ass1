#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include "CircleBuffer.h"

int main(int argc, char*argV[])
{
	DWORD delay = atoi(argV[2]);
	size_t buffSize = atoi(argV[3]);
	size_t memorySize = atoi(argV[4]);
	size_t numMessages = atoi(argV[5]);
	size_t msgSize;
	if (std::strcmp("random", argV[6]))
	{
		msgSize = rand();
	}
	else
	{
		msgSize = atoi(argV[6]);
	}

	if (std::strcmp("producer", argV[1]))
	{
		printf("produceeeeer, %d %d %d %d %d", delay, buffSize,numMessages,memorySize,msgSize);
	}

	if (std::strcmp("consumer", argV[1]))
	{
		printf("consuuuuumeer, %d %d %d %d %d", delay, buffSize, numMessages, memorySize, msgSize);
	}

	getchar();
	getchar();
}

void producer()
{

}
void consumer()
{

}