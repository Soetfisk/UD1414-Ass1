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
	this->chunkCount = 0;

	bool * nums = new bool[buffSize];
	for (int i = 0; i < buffSize; i += chunkSize)
		chunkCount++;
	delete nums;

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

	memcpy((char*)&controller, cData, sizeof(Control));

	if (isProducer)
	{
		if (controller.Head == nullptr)
		{
			controller.Head = (char*)mData;
			memcpy(cData, (char*)&controller, sizeof(char*));
		}
	}
	else {
		controller.clients++;
		if (controller.Tail == nullptr)
		{
			controller.Tail = (char*)mData;
			memcpy(cData, ((char*)&controller+sizeof(char*)), sizeof(char*)+sizeof(unsigned int));
		}
		else {
			memcpy(cData, ((char*)&controller + (sizeof(char*)*2)), sizeof(unsigned int));
		}
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

	controller.Head++;

	size_t temp = (controller.Head - mData);
	temp = (controller.Tail - mData);
	temp = (buffSize - (controller.Head - mData));

	if ((buffSize - (controller.Head - mData) - (controller.Tail - mData)) > msgSize) //check if there's enough space ahead
	{
		if ((buffSize - (controller.Head - mData)) < msgSize) //check if we're going past the end with the message
			controller.Head = (char*)mData;			//if so: we move the Head to the beginning.

		Header h;
		h.id = this->id;
		h.length = length;
		h.padding = msgSize - (length + sizeOfHeader);

		memcpy(controller.Head, &h, sizeOfHeader);
		controller.Head += sizeOfHeader;
		memcpy(controller.Head, msg, (msgSize - sizeOfHeader)); //offset on pointer, how?
		controller.Head += (msgSize - sizeOfHeader);

		//controller.Head++;									     // mutex check not needed, there's just one head
		memcpy((char*)cData, &controller.Head, sizeof(char*));	 // only write the head data

		this->id++;

		return true;
	}
	else
		return false;
}

bool CircleBuffer::pop(char * msg, size_t & length)
{
	//Header h;
	//memcpy(&h, mData, sizeof(Header));
	//memcpy()

	//if ()

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