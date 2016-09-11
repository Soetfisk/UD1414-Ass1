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

void gen_random(char *s, const int len) {
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	for (auto i = 0; i < len; ++i) {
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}
	s[len] = 0;
}


void producer(DWORD delay, size_t memorySize, size_t numMessages, size_t msgSize)
{
	CircleBuffer producer(L"uniqueName", memorySize, true, 256);

	//while (producer.tryConnect())
	//	Sleep(100);

	size_t totalSent = 0;
	int counter = 0;

	if (msgSize == 0 /*|| msgSize > ((memorySize / 4)- 32)*/)
		msgSize = (memorySize / 4) - 32;
	else
		msgSize -= 32;

	char* buff = new char[msgSize];

	while (numMessages != counter)
	{
		if (delay > 0)
			Sleep(delay);

		gen_random(buff, msgSize);
		while (true)
		{
			if (producer.push(buff, msgSize))
			{
				counter++;
				break;
			}
			else
				Sleep(1);
		}
	}
	delete buff;
}
void consumer(DWORD delay, size_t memorySize, size_t numMessages, size_t msgSize)
{
	CircleBuffer consumer(L"uniqueName", memorySize, false, msgSize);
	//assert(consumer.isValid())

	if (msgSize == 0 /*|| msgSize > (memorySize / 4)-32*/)
		msgSize = (memorySize / 4) - 32;
	else
		msgSize -= 32;

	char * msg = new char[msgSize]; 
	size_t len;
	size_t counter = 0;

	while (numMessages < counter)
	{
		if (delay > 0)
			Sleep(delay);
		if (consumer.pop(msg, len)) //using a &* could make the ringbuffer able to handle initalization instead
		{
			counter++;
			//msg = new char[len];
			printf("%d. %s\n",counter, msg);
			//delete msg;
		}
		else
			Sleep(1);
	}

	delete msg;
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

	//getchar();

	return 0;
}

