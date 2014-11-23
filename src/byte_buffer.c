#include <string.h>

#include "byte_buffer.h"
#include "memory_allocations.h"


ByteBuffer *AllocateByteBuffer (size_t initial_size)
{
	STRPTR data_p = (STRPTR) IExec->AllocVecTags (initial_size, TAG_DONE);

	if (data_p)
		{
			ByteBuffer *buffer_p = (ByteBuffer *) IExec->AllocVecTags (sizeof (struct ByteBuffer), TAG_DONE);

			if (buffer_p)
				{
					buffer_p -> bb_data_p = data_p;
					buffer_p -> bb_size  = initial_size;
					buffer_p -> bb_current_index = 0;

					return buffer_p;
				}

			IExec->FreeVec (data_p);
		}

	return NULL;
}


void FreeByteBuffer (ByteBuffer *buffer_p)
{
	IExec->FreeVec (buffer_p -> bb_data_p);
	IExec->FreeVec (buffer_p);
}


BOOL ExtendByteBuffer (ByteBuffer *buffer_p, size_t increment)
{
	return ResizeByteBuffer (buffer_p, (buffer_p -> bb_size) + increment);
}


BOOL ResizeByteBuffer (ByteBuffer *buffer_p, size_t new_size)
{
	BOOL success_flag = FALSE;

	STRPTR new_data_p = (STRPTR) IExec->AllocVecTags (new_size, TAG_DONE);

	if (new_data_p)
		{
			if (new_size > buffer_p -> bb_current_index)
				{
					memcpy (new_data_p, buffer_p -> bb_data_p, buffer_p -> bb_current_index);
					* (buffer_p -> bb_data_p + (buffer_p -> bb_current_index)) = '\0';
				}

			IExec->FreeVec (buffer_p -> bb_data_p);

			buffer_p -> bb_data_p = new_data_p;
			buffer_p -> bb_size = new_size;

			success_flag = TRUE;
		}

	return success_flag;
}


BOOL AppendToByteBuffer (struct ByteBuffer *buffer_p, const void *data_p, const size_t data_length)
{
	const size_t space_remaining = GetRemainingSpaceInByteBuffer (buffer_p);
	BOOL success_flag = TRUE;

	if (space_remaining <= data_length)
		{
			success_flag = ResizeByteBuffer (buffer_p, (buffer_p -> bb_size) + data_length - space_remaining);
		}

	if (success_flag)
		{
			char *current_data_p = (buffer_p -> bb_data_p) + (buffer_p -> bb_current_index);

			memcpy (current_data_p, data_p, data_length);
			buffer_p -> bb_current_index += data_length;
		}

	return success_flag;
}


void ResetByteBuffer (struct ByteBuffer *buffer_p)
{
	buffer_p -> bb_current_index = 0;
}


size_t GetRemainingSpaceInByteBuffer (const struct ByteBuffer * const buffer_p)
{
	return (buffer_p -> bb_size) - (buffer_p -> bb_current_index);
}


BOOL MakeByteBufferDataValidString (struct ByteBuffer *buffer_p)
{
	BOOL success_flag = TRUE;

	if (buffer_p -> bb_current_index == (buffer_p -> bb_size)- 1)
		{
			success_flag = ResizeByteBuffer (buffer_p, (buffer_p -> bb_size) + 1);
		}

	if (success_flag)
		{
			* ((buffer_p -> bb_data_p) + (buffer_p -> bb_current_index)) = '\0';
		}

	return success_flag;
}