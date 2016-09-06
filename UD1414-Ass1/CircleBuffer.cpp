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
	this->buffName = buffName;
	this->buffSize = buffSize;
	this->chunkSize = chunkSize;
	id = 0;

	LPCWSTR c = L"Control";
	LPCWSTR m = L"Mutex";
	std::wstring tempwstr;

	tempwstr = std::wstring(buffName) + c;
	LPCWSTR controlName = tempwstr.c_str();

	tempwstr = std::wstring(buffName) + m;
	LPCWSTR mutexName = tempwstr.c_str();

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
		controlName
	);
	cData = MapViewOfFile(hControl, FILE_MAP_ALL_ACCESS, 0, 0, 0);

	hMutex = CreateMutex(NULL, false, mutexName);

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
	int sizeOfHeader = sizeof(Header);
	size_t msgSize = roundUp((length + sizeOfHeader), chunkSize);

	memcpy((char*)&controller, cData, sizeof(Control)); //woop! looks like this works, references ftw

	if ((buffSize - ((controller.Head - controller.Tail) * chunkSize)) > msgSize) //controller writespace check
	{
		Header h;
		h.id = this->id;
		h.length = length;
		h.padding = msgSize - (length + sizeOfHeader);

		memcpy((char*)mData, &h, sizeOfHeader);
		memcpy((char*)mData, msg, (msgSize - sizeOfHeader)); //offset on pointer, how?

		controller.Head++;									     // mutex check not needed, there's just one head
		memcpy((char*)cData, &controller.Head, sizeof(size_t));	 // only write the head data

		this->id++;

		return true;
	}
	else
		return false;
}

bool CircleBuffer::pop(char * msg, size_t & length)
{
	Header h;
	memcpy(&h, mData, sizeof(Header));
	memcpy()

	if ()

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