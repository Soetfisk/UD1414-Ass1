#include "CircleBuffer.h"

CircleBuffer::CircleBuffer()
{
}

inline size_t CircleBuffer::roundUp(size_t num, size_t multiple)
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
	//this->chunkCount = 0;

	//bool * nums = new bool[buffSize];
	//for (int i = 0; i < buffSize; i += chunkSize)
	//	chunkCount++;
	//delete nums;


	DWORD dw;
	LPCWSTR c = L"Control";
	LPCWSTR m = L"Mutex";
	std::wstring tempwstrc, tempwstrm;

	tempwstrc = std::wstring(buffName) + c;
	LPCWSTR controlName = tempwstrc.c_str();

	tempwstrm = std::wstring(buffName) + m;
	LPCWSTR mutexName = tempwstrm.c_str();

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
		controller[2] = buffSize;
		controller[3] = 0;
	}


	HEAD = controller;
	TAIL = HEAD + 1;
	FREEMEM = TAIL + 1;
	CLIENTS = FREEMEM + 1;

	if (isProducer)
	{

	}
	else {
		//mutexlock
		CLIENTS++;
		//mutexunlock
	}


}

CircleBuffer::~CircleBuffer()
{
}

size_t CircleBuffer::canRead()
{
	return size_t();
}

size_t CircleBuffer::canWrite()
{
	return size_t();
}

bool CircleBuffer::push(const void * msg, size_t length)
{
	static size_t msgId = 0;
	static size_t freeMem = buffSize;
	size_t msgSize = roundUp(length + sizeOfHeader);


	if (*TAIL > *HEAD)
		freeMem = (*TAIL - *HEAD);
	else if (*TAIL < *HEAD)
		freeMem = ((buffSize -*HEAD) - *TAIL);

	/*if (msgSize < (*FREEMEM - 1))*/
	if (msgSize < freeMem)
	{
		Header h
		{
			msgId++,
			length,
			(size_t)(msgSize - length),
			(size_t)0
		};

		memcpy(mData + *HEAD, &h, sizeOfHeader);
		memcpy((mData + *HEAD + sizeOfHeader), msg, msgSize - sizeOfHeader);
		freeMem -= msgSize;

		*HEAD = (*HEAD + msgSize) % buffSize;
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

		//add mutex and check if the tail's gonna move for multiple client supports
		h->readCount++;
		if (h->readCount == *CLIENTS)
			*TAIL = localTail;
		//if next head (tailpos == headpos), wait for it to move
		return	true;
	}
	else
		return false;

}

bool CircleBuffer::isValid()
{
	if (hData == NULL)
	{
		//hData = OpenFileMapping(FILE_MAP_ALL_ACCESS, false, this->buffName);
		//tail = MapViewOfFile(hData, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		return false;
	}
	else
		return true;
}