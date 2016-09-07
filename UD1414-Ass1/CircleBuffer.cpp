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
		//mutexlock

		if (controller.Tail == nullptr)
		{
			controller.Tail = (char*)mData;
			memcpy(cData, ((char*)&controller+sizeof(char*)), sizeof(char*)+sizeof(size_t));
		}
		else {
			memcpy(cData, ((char*)&controller + (sizeof(char*)*2)), sizeof(size_t));
		}

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
	int sizeOfHeader = sizeof(Header);
	size_t msgSize = roundUp((length + sizeOfHeader), chunkSize) - sizeOfHeader;


	//if ((buffSize - hPos) < msgSize)					  //check if we're going past the end with the message
	//	controller.Head = (char*)mData;					  //if so: we move the Head to the beginning.
	//else
	//	controller.Head += sizeOfHeader;;

	memcpy((char*)&controller, cData, sizeof(Control)); //woop! looks like this works, references ftw


	// size_t temp = (controller.Head - mData); <- this will give you the number of bytes the head has moved.

	size_t tPos = (controller.Tail - mData);
	size_t hPos = (controller.Head - mData);
	size_t htForwardDistance; //Available space forward, from head to tail

	if (hPos < tPos)
		htForwardDistance = tPos - hPos;
	else
		htForwardDistance = (buffSize - (hPos - tPos));

	if (htForwardDistance > msgSize + sizeOfHeader) //is there room for the message?
	{
		Header h;
		h.id = this->id;
		h.length = length;
		h.padding = msgSize - (length + sizeOfHeader);

		memcpy(controller.Head, &h, sizeOfHeader);


		if ((buffSize - sizeOfHeader - hPos) > (msgSize)) //check if we're going past the end with the message
		{												  //if so: we move the Head to the beginning.
			controller.Head += sizeOfHeader;
		}
		else
		{
			controller.Head = (char*)mData;
		}

		memcpy(controller.Head, msg, (msgSize));
		controller.Head += (msgSize);

																	 // mutex check not needed, there's just one head
		memcpy((char*)cData, &controller.Head, sizeof(char*));		 // only write the head data into control memory

		this->id++;													 //iterate msg id

		return true;


	}
	else
		return false;
}

bool CircleBuffer::pop(char * msg, size_t & length)
{

	Header h;
	int sizeOfHeader = sizeof(Header);


	memcpy((char*)&controller, cData, sizeof(Control));		//read latest control data, perhaps mutexlock? Unless char* are Atomic, you don't want a corrupted char*.
	memcpy(controller.Head, &h, sizeof(Header));

	size_t msgSize = roundUp(h.length + sizeof(Header), chunkSize) - sizeOfHeader;

	if ((buffSize - sizeOfHeader) - (controller.Tail - mData) < msgSize)					//för o resetta på första platesen, kolla med length om den avrundade storleken hade överstigit minnesutrymmet. 
		controller.Tail = (char*)mData;														//Att använda paremeter length är fewlt men vafan

	memcpy(&h, controller.Tail, sizeof(Header));

	h.readCount++;
	

	memcpy(msg, (controller.Tail + sizeof(Header)), (h.length + h.padding));

	//MOVE TAIL STUFF
	
	if (h.readCount == controller.clients)
	{
		if (controller.Tail == mData);
			//nothing	
		else
			controller.Tail += msgSize;
		//MUTEXLOCK
		memcpy(cData, ((char*)&controller + sizeof(char*)), (sizeof(char*) + sizeof(size_t)));
		//MUTEXUNLOCK
	}
	else {
		//MUTEXLOCK
		memcpy(cData, ((char*)&controller + (sizeof(char*) * 2)), sizeof(size_t));
		//MUTEXUNLOCK
	}

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