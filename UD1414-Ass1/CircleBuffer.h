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
		size_t * Head;
		size_t * Tail;
		size_t * freeMem;
		size_t * clients;
	};
	// your private stuff,
	// implementation details, etc.
	//

	HANDLE hData, hControl, hMutex;
	size_t buffSize, chunkSize/*, chunkCount*/;
	size_t * HEAD, * TAIL, * FREEMEM, * CLIENTS; //USE MUTEX IN PRODUCER WHEN WRITING
	char* mData;
	Control * controller;

	

	struct Header
	{
		size_t id;
		size_t length;
		size_t padding; // optional
		size_t readCount;
	};

	inline size_t roundUp(size_t num, size_t multiple = 256);


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

