#include "CircleBuffer.h"

CircleBuffer::CircleBuffer()
{
}

size_t CircleBuffer::roundUp(size_t num, size_t multiple)
{
	assert(multiple);
	return ((num + multiple - 1) / multiple) * multiple;
}

CircleBuffer::CircleBuffer(LPCWSTR buffName, const size_t & buffSize, const bool & isProducer, const size_t & chunkSize)
{
	this->hControl = NULL;
	this->hData = NULL;
	this->buffSize = buffSize;
	this->chunkSize = chunkSize;

	DWORD dw;
	LPCWSTR c = L"Control";
	LPCWSTR m = L"Mutex";
	std::wstring tempwstrc, tempwstrm;

	tempwstrc = std::wstring(buffName) + c;
	LPCWSTR controlName = tempwstrc.c_str();

	tempwstrm = std::wstring(buffName) + m;
	LPCWSTR mutexName = tempwstrm.c_str();

	hMutex = CreateMutex(NULL, false, mutexName);
	
	hData = CreateFileMapping
	(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		buffSize,
		buffName
	);
	
	mData = (char*)MapViewOfFile(hData, FILE_MAP_ALL_ACCESS, 0, 0, buffSize);

	hControl = CreateFileMapping
	(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(size_t)*3,
		controlName
	);

	dw = GetLastError();

	controller = (size_t*)MapViewOfFile(hControl, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(size_t) * 3);
	
	if (!(dw == ERROR_ALREADY_EXISTS))
	{
		controller[0] = 0;
		controller[1] = 0;
		controller[2] = 0;
	}

	HEAD = controller;
	TAIL = HEAD + 1;
	CLIENTS = TAIL + 1;

	if (!isProducer)
	{
		//WaitForSingleObject(hMutex, INFINITE); //mutexlock
		*CLIENTS += 1;
		//ReleaseMutex(hMutex); //mutexunlock
	}
}

CircleBuffer::~CircleBuffer()
{
}

bool CircleBuffer::push(const void * msg, size_t length)
{
	static size_t msgId = 0;
	static size_t freeMem = buffSize;
	static size_t lastTail = *TAIL;
	size_t msgSize = roundUp(length + sizeOfHeader);
	size_t localTail = *TAIL;

	if (localTail > *HEAD)
		freeMem = (localTail - *HEAD);
	else if (localTail < *HEAD)
		freeMem = ((buffSize - *HEAD) - localTail);
	//else if (localTail == *HEAD && lastTail != localTail)
	//	freeMem = buffSize;


	if (msgSize <= freeMem)
	{
		lastTail = localTail;
		size_t nextPos = (*HEAD + msgSize) % buffSize;
		if (nextPos == localTail)
			return false;

		Header h
		{
			msgId++,
			length,
			(size_t)(msgSize - length),
			(size_t)0
		};

		memcpy(mData + *HEAD, &h, sizeOfHeader);
		memcpy((mData + *HEAD + sizeOfHeader), msg, length);
		freeMem -= msgSize;

		*HEAD = nextPos;
		return true;
	}
	else
		return false;
}

bool CircleBuffer::pop(char * msg, size_t & length)
{
	static size_t localTail = *TAIL;
	//static Header * h = nullptr;

	if (localTail != *HEAD)
	{	
		Header * h = (Header*)(mData + localTail);
		length = h->length;
		size_t nextPos = (localTail + length + h->padding) % buffSize;
		if (nextPos == *HEAD)
			return false;

		memcpy(msg, mData + localTail + sizeOfHeader, length);

		localTail = nextPos;

		WaitForSingleObject(hMutex, INFINITE);
		h->readCount++;
		ReleaseMutex(hMutex);

		if (h->readCount == *CLIENTS)
			*TAIL = localTail;
		return	true;
	}
	else
		return false;
}