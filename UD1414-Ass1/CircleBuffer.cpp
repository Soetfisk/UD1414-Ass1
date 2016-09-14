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
	LPCWSTR m = L"mMutex";
	LPCWSTR cl = L"clMutex";
	LPCWSTR r = L"rMutex";
	std::wstring tempwstrc, tempwstrmm, tempwstrmc, tempwstrmr;

	tempwstrc = std::wstring(buffName) + c;
	LPCWSTR controlName = tempwstrc.c_str();

	tempwstrmm = std::wstring(buffName) + m;
	LPCWSTR memMutex = tempwstrmm.c_str();

	tempwstrmc = std::wstring(buffName) + m;
	LPCWSTR cliMutex = tempwstrmc.c_str();

	tempwstrmr = std::wstring(buffName) + m;
	LPCWSTR reaMutex = tempwstrmr.c_str();


	hMemMutex = CreateMutex(NULL, false, memMutex);
	hClientMutex = CreateMutex(NULL, false, cliMutex);
	hReadMutex = CreateMutex(NULL, false, reaMutex);

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
		sizeof(size_t)*4,
		controlName
	);

	dw = GetLastError();

	controller = (size_t*)MapViewOfFile(hControl, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(size_t) * 4);
	
	if (!(dw == ERROR_ALREADY_EXISTS))
	{
		controller[0] = 0;
		controller[1] = 0;
		controller[2] = 0;
		controller[4] = buffSize;
	}

	HEAD = controller;
	TAIL = HEAD + 1;
	CLIENTS = TAIL + 1;
	FREEMEM = &controller[4];

	if (!isProducer)
	{
		WaitForSingleObject(hClientMutex, INFINITE); //mutexlock
		*CLIENTS += 1;
		ReleaseMutex(hClientMutex); //mutexunlock
	}
}

CircleBuffer::~CircleBuffer()
{
}

bool CircleBuffer::push(const void * msg, size_t length)
{
	
	static size_t msgId = 0;
	WaitForSingleObject(hMemMutex, INFINITE);
	size_t freeMem = *FREEMEM;
	ReleaseMutex(hMemMutex);
	static size_t lastTail = *TAIL;
	size_t msgSize = roundUp(length + sizeOfHeader, chunkSize);
	size_t localTail = *TAIL;


	if (msgSize <= (freeMem-1))
	{

		lastTail = localTail;

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
		
		WaitForSingleObject(hMemMutex, INFINITE);
		*FREEMEM = freeMem;
		ReleaseMutex(hMemMutex);
		*HEAD = (*HEAD + msgSize) % buffSize;;

		return true;
	}
	else
		return false;
}

bool CircleBuffer::pop(char * msg, size_t & length)
{
	
	static size_t localTail = *TAIL;

	if (localTail != *HEAD)
	{	
	
		Header * h = (Header*)(mData + localTail);
		length = h->length;

		memcpy(msg, mData + localTail + sizeOfHeader, length);

		localTail = (localTail + length + h->padding) % buffSize;

		WaitForSingleObject(hReadMutex, INFINITE);
		h->readCount++;

		if (h->readCount == *CLIENTS)
		{
			*FREEMEM = *FREEMEM + (length + h->padding);
			*TAIL = localTail;
		}
		ReleaseMutex(hReadMutex);
		return	true;
	}
	else
		return false;
}