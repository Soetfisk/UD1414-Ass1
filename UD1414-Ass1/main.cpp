#include "CircleBuffer.h"



size_t random(size_t min, size_t max)
{
	size_t range, result, cutoff;

	if (min >= max)
		return min;
	range = max - min + 1;
	cutoff = (RAND_MAX / range) * range;

	do {
		result = rand();
	} while (result >= cutoff);

	return result % range + min;
}

size_t randomString(char *s, const size_t maxSize) {

	size_t rLen = random(1, maxSize);
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	for (int i = 0; i < rLen; ++i) {
		s[i] = alphanum[random(1, sizeof(alphanum))];
	}

	s[rLen] = '\0';

	return rLen;
}



void producer(DWORD delay, size_t memorySize, size_t numMessages, size_t msgSize)
{
	CircleBuffer producer(L"uniqueName", memorySize, true, 256);



	//while (producer.tryConnect())
	//	Sleep(100);

	size_t randomSize = 0;
	size_t totalSent = 0;
	int counter = 0;

	if (msgSize == 0 /*|| msgSize > (memorySize / 4)*/)
		msgSize = (memorySize / 4);

	char* buff = new char[msgSize];

	while (numMessages != counter)
	{
		if (delay > 0)
			Sleep(delay);

		randomSize = randomString(buff, msgSize);
		while (true)
		{
			if (producer.push(buff, randomSize))
			{
				printf("%s\n", buff);
				totalSent += randomSize;
				counter++;
				break;
			}
			else
				Sleep(1);
		}
	}
}
void consumer(DWORD delay, size_t memorySize, size_t numMessages, size_t msgSize)
{
	CircleBuffer consumer(L"uniqueName", memorySize, false, msgSize);
	//assert(consumer.isValid())

	char msg;
	size_t len;

	while (true)
	{
		if (delay > 0)
			Sleep(delay);
		if (consumer.pop(&msg, len))
		{
			//msg = new char[len];
			printf("%s\n", msg);
			//delete msg;
			break;
		}

		else
			Sleep(1);
	}
}

int main(int argc, char*argv[])
{
	srand(time(NULL));

	DWORD delay = atoi(argv[2]);
	size_t memorySize = atoi(argv[3]);
	size_t numMessages = atoi(argv[4]);
	
	size_t msgSize;
	if (std::strcmp("random", argv[5]) == 0 || argv[5] == nullptr)
	{
		msgSize = 0;
	}
	else
	{
		msgSize = atoi(argv[5]);
	}

	if (std::strcmp("producer", argv[1]) == 0)
	{
		//printf("produceeeeer, %d %d %d %d\n", delay, memorySize, numMessages, msgSize);
		producer(delay, memorySize << 10, numMessages, msgSize);
	}

	if (std::strcmp("consumer", argv[1]) == 0)
	{
		//printf("consuuuuumeer, %d %d %d %d\n", delay, memorySize, numMessages, msgSize);
		consumer(delay, memorySize << 10, numMessages, msgSize);
	}

	getchar();

	return 0;
}

