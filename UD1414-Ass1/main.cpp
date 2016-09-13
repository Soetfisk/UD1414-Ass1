#include "CircleBuffer.h"

inline void gen_random(char *s, const int len) {
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	for (auto i = 0; i < len; ++i) {
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}
	s[len-1] = '\0';
}

void producer(DWORD delay, size_t memorySize, size_t numMessages, size_t msgSize)
{
	CircleBuffer producer(L"uniqueName", memorySize, true, 256);

	size_t totalSent = 0;
	int counter = 0;

	if (msgSize == 0 /*|| msgSize > ((memorySize / 4)- 32)*/)
		msgSize = (memorySize / 4) - producer.sizeOfHeader;
	else
		msgSize -= producer.sizeOfHeader;

	char* buff = new char[msgSize];

	while (numMessages >= counter)
	{
		if (delay > 0)
			Sleep(delay);

		gen_random(buff, msgSize);
		while (true)
		{
			if (producer.push(buff, msgSize))
			{
				counter++;
				printf("%d. %s\n", counter, buff);
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
	CircleBuffer consumer(L"uniqueName", memorySize, false, 256);

	if (msgSize == 0 /*|| msgSize > (memorySize / 4)-32*/)
		msgSize = (memorySize / 4) - consumer.sizeOfHeader;
	else
		msgSize -= consumer.sizeOfHeader;

	char * msg = new char[msgSize]; 
	size_t len;
	size_t counter = 0;

	while (numMessages >= counter)
	{
		if (delay > 0)
			Sleep(delay);
		if (consumer.pop(msg, len)) //using a &* could make the ringbuffer able to handle initalization instead
		{
			counter++;
			printf("%d. %s\n",counter, msg);
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
		producer(delay, memorySize << 10, numMessages, msgSize);
	}

	if (std::strcmp("consumer", argv[1]) == 0)
	{
		consumer(delay, memorySize << 10, numMessages, msgSize);
	}

	return 0;
}

