#ifndef BYTE_BUFFER_H
#define BYTE_BUFFER_H

#include <stddef.h>

#include <exec/types.h>

typedef struct ByteBuffer
{
	STRPTR bb_data_p;
	size_t bb_size;
	size_t bb_current_index;
} ByteBuffer;




struct ByteBuffer *AllocateByteBuffer (size_t initial_size);


void FreeByteBuffer (struct ByteBuffer *buffer_p);


bool ResizeByteBuffer (struct ByteBuffer *buffer_p, size_t new_size);


bool ExtendByteBuffer (struct ByteBuffer *buffer_p, size_t increment);


bool AppendToByteBuffer (struct ByteBuffer *buffer_p, const void *data_p, const size_t data_length);


void ResetByteBuffer (struct ByteBuffer *buffer_p);


bool MakeByteBufferDataValidString (struct ByteBuffer *buffer_p);


size_t GetRemainingSpaceInByteBuffer (const struct ByteBuffer * const buffer_p);


#endif		/* #ifndef BYTE_BUFFER_H */