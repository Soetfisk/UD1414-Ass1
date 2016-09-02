#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include "CircleBuffer.h"

int main(int argc, char*argv[])
{
	DWORD delay = atoi(argv[2]);
	size_t buffSize = atoi(argv[3]);
	size_t memorySize = atoi(argv[4]);
	size_t numMessages = atoi(argv[5]);
	unsigned int msgSize;
	if (std::strcmp("random", argv[6]))
	{
		msgSize = rand();
	}
	else 
	{
		msgSize = atoi(argv[6]);
	}



	if (std::strcmp("producer", argv[1]))
	{

	}

	if (std::strcmp("consumer", argv[1]))
	{

	}
}

void producer()
{

}
void consumer()
{

}