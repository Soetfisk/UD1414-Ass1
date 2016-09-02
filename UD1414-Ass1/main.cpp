#include "CircleBuffer.h"

int main(int argc, char*argv[])
{
	
	DWORD delay = atoi(argv[2]);
	size_t buffSize = atoi(argv[3]);
	size_t memorySize = atoi(argv[4]);
	size_t numMessages = atoi(argv[5]);
	
	size_t msgSize;
	if (std::strcmp("random", argv[6]))
	{
		msgSize = rand() % (memorySize/4) + 1;
	}
	else
	{
		msgSize = atoi(argv[6]);
	}
	

	if (std::strcmp("producer", argv[1]))
	{
		printf("produceeeeer, %d %d %d %d %d", delay, buffSize, memorySize,numMessages,msgSize);
	}

	if (std::strcmp("consumer", argv[1]))
	{
		printf("consuuuuumeer, %d %d %d %d %d", delay, buffSize, memorySize, numMessages, msgSize);
	}

	
	return 0;
}

void producer()
{

}
void consumer()
{

}