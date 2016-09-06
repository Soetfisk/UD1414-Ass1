#include "CircleBuffer.h"


CircleBuffer::CircleBuffer()
{
}

CircleBuffer::CircleBuffer(LPCWSTR buffName, const size_t & buffSize, const bool & isProducer, const size_t & chunkSize)
{
	this->hControl = NULL;
	this->hData = NULL;
	this->buffName = buffName;
	this->buffSize = buffSize;
	this->chunkSize = chunkSize;


	hData = CreateFileMapping
	(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		(DWORD)0,
		buffSize,
		buffName
	);
	mData = MapViewOfFile(hData, FILE_MAP_ALL_ACCESS, 0, 0, 0);

	hControl = CreateFileMapping
	(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		(DWORD)0,
		sizeof(Control),
		(buffName + L'c')
	);
	cData = MapViewOfFile(hControl, FILE_MAP_ALL_ACCESS, 0, 0, 0);

	if (isProducer)
	{

	}
	else {

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
	return true;
}

bool CircleBuffer::pop(char * msg, size_t & length)
{
	return true;
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