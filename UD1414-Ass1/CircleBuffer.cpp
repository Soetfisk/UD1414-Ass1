#include "CircleBuffer.h"

CircleBuffer::CircleBuffer()
{

}

CircleBuffer::CircleBuffer(LPCWSTR buffName, const size_t & buffSize, const bool & isProducer, const size_t & chunkSize)
{
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
	return false;
}

bool CircleBuffer::pop(char * msg, size_t & length)
{
	return false;
}
