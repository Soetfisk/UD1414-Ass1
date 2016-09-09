#pragma once

#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <assert.h>
#include <time.h>

#ifndef CIRCLEBUFFER_H
#define CIRCLEBUFFER

//testa med en memcopy till och från på ett ställe i minnet

class CircleBuffer
{
private:
	struct Control
	{
		char* Head = nullptr;
		char* Tail = nullptr;
		size_t* clients;
	};
	// your private stuff,
	// implementation details, etc.
	//

	size_t id;
	HANDLE hData, hControl, hMutex;
	LPCWSTR buffName;
	size_t buffSize, chunkSize, chunkCount;
	void *mData, * cData;
	Control * controller;



	struct Header
	{
		size_t id;
		size_t length;
		size_t padding; // optional
		size_t readCount = 0;// maybe number of consumers here?
	};

	size_t roundUp(size_t num, size_t multiple = 256);


public:
	// Constructor
	CircleBuffer(
		LPCWSTR buffName,          // unique name
		const size_t& buffSize,    // size of the whole filemap
		const bool& isProducer,    // is this buffer going to be used as producer
		const size_t& chunkSize);  // round up messages to multiple of this.

								   // Destructor
	CircleBuffer();
	~CircleBuffer();

	size_t sizeOfHeader = sizeof(Header);		//UGLYHAX

	bool isValid();    // return true if there's a valid handle
	size_t canRead();  // returns how many bytes are available for reading.
	size_t canWrite(); // returns how many bytes are free in the buffer.
					   // try to send a message through the buffer,
					   // if returns true then it succeeded, otherwise the message has not been sent.
					   // it should return false if the buffer does not have enough space.

	// try to send a message through the buffer,
	// if returns true, then it succeeded, otherwise the message has not been sent.
	// it should return false if the buffer does not have enough space.
	bool push(const void* msg, size_t length);
	// try to read a message from the buffer, and the implementation puts the content
	// in the memory. The memory is expected to be allocated by the program that calls
	// this function.
	// The variable length will be changed to reflect the size of the msg just read.
	bool pop(char* msg, size_t& length);
};



#endif // !CIRCLEBUFFER_H

