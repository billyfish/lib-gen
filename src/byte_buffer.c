#include <string.h>
#include <stdlib.h>

#include <proto/exec.h>

#include "byte_buffer.h"
#include "utils.h"
#include "debugging_utils.h"


#ifdef _DEBUG
#define BYTE_BUFFER_DEBUG (10)
#endif



ByteBuffer *AllocateByteBuffer (size_t initial_size)
{
	ENTER ();

	STRPTR data_p = (STRPTR) IExec->AllocVecTags (initial_size, AVT_ClearWithValue, 0, TAG_DONE);

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

	LEAVE ();

	return NULL;
}


void FreeByteBuffer (ByteBuffer *buffer_p)
{
	ENTER ();

	IExec->FreeVec (buffer_p -> bb_data_p);
	IExec->FreeVec (buffer_p);

	LEAVE ();
}


BOOL ExtendByteBuffer (ByteBuffer *buffer_p, size_t increment)
{
	ENTER ();

	BOOL b = ResizeByteBuffer (buffer_p, (buffer_p -> bb_size) + increment);

	LEAVE ();

	return b;
}


BOOL ResizeByteBuffer (ByteBuffer *buffer_p, size_t new_size)
{
	ENTER ():

	BOOL success_flag = FALSE;
	STRPTR new_data_p = (STRPTR) IExec->AllocVecTags (new_size, AVT_ClearWithValue, 0, TAG_DONE);

	#if BYTE_BUFFER_DEBUG >= 10
	DB (KPRINTF ("%s %ld - Pre-resize:\n", __FILE__, __LINE__));
	DebugPrintByteBuffer (buffer_p);
	#endif


	if (new_data_p)
		{
			if (new_size > buffer_p -> bb_current_index)
				{
					memcpy (new_data_p, buffer_p -> bb_data_p, buffer_p -> bb_current_index);
				}

			IExec->FreeVec (buffer_p -> bb_data_p);

			buffer_p -> bb_data_p = new_data_p;
			buffer_p -> bb_size = new_size;

			success_flag = TRUE;
		}

	#if BYTE_BUFFER_DEBUG >= 10
	DB (KPRINTF ("%s %ld - Post-resize: %ld\n", __FILE__, __LINE__, success_flag));
	DebugPrintByteBuffer (buffer_p);
	#endif

	LEAVE ();

	return success_flag;
}


BOOL AppendToByteBuffer (struct ByteBuffer *buffer_p, const void *data_p, const size_t data_length)
{
	ENTER ();

	const size_t space_remaining = GetRemainingSpaceInByteBuffer (buffer_p);
	BOOL success_flag = TRUE;

	#if BYTE_BUFFER_DEBUG >= 10
	DB (KPRINTF ("%s %ld - data to append \"%s\" length %lu\n", __FILE__, __LINE__, (const char *) data_p, data_length));
	DB (KPRINTF ("%s %ld - Pre-append size remaining: %lu\n", __FILE__, __LINE__, space_remaining));
	DebugPrintByteBuffer (buffer_p);
	#endif

	if (space_remaining <= data_length)
		{
			success_flag = ResizeByteBuffer (buffer_p, (buffer_p -> bb_size) + data_length - space_remaining);
		}

	if (success_flag)
		{
			char *current_data_p = (buffer_p -> bb_data_p) + (buffer_p -> bb_current_index);

			memcpy (current_data_p, data_p, data_length);
			buffer_p -> bb_current_index += data_length;

			current_data_p = (buffer_p -> bb_data_p) + ((buffer_p -> bb_current_index) - 1);
			if ((*current_data_p == '\r') || (*current_data_p == '\n'))
				{
					*current_data_p = ' ';
				}
		}

	#if BYTE_BUFFER_DEBUG >= 10
	DB (KPRINTF ("%s %ld - post-append success_flag: %ld\n", __FILE__, __LINE__, success_flag));
	DebugPrintByteBuffer (buffer_p);
	#endif

	LEAVE ();

	return success_flag;
}


void ResetByteBuffer (struct ByteBuffer *buffer_p)
{
	ENTER ();

	buffer_p -> bb_current_index = 0;
	memset (buffer_p -> bb_data_p, 0, buffer_p -> bb_size);

	LEAVE ();
}


size_t GetRemainingSpaceInByteBuffer (const struct ByteBuffer * const buffer_p)
{
	ENTER ();

	LEAVE ();

	return (buffer_p -> bb_size) - (buffer_p -> bb_current_index);
}


STRPTR ExtractSubstring (struct ByteBuffer *buffer_p, char *delim_p)
{
	ENTER ();

	char *substring_s = NULL;
	size_t sub_length = (size_t) (delim_p - (buffer_p -> bb_data_p));

	/* cut the string from the buffer */
	substring_s = CopyToNewString (buffer_p -> bb_data_p, delim_p, FALSE);

	#if BYTE_BUFFER_DEBUG >= 10
	DB (KPRINTF ("%s %ld - ********** pre-extract: substring \"%s\" length %lu\n", __FILE__, __LINE__, substring_s, sub_length));
	DebugPrintByteBuffer (buffer_p);
	#endif


	if (substring_s)
		{
			if (sub_length <= buffer_p -> bb_current_index)
				{
					size_t remaining_length = (buffer_p -> bb_current_index) - sub_length - 1;
					char *dest_p = NULL;

					#if BYTE_BUFFER_DEBUG >= 10
					DB (KPRINTF ("%s %ld - pre-extract: remaining_length \"%ld\"\n", __FILE__, __LINE__, remaining_length));
					#endif

					dest_p = memmove (buffer_p -> bb_data_p, delim_p + 1, remaining_length);

					if (dest_p)
						{
							* (dest_p + remaining_length) = '\0';
							buffer_p -> bb_current_index = remaining_length;
							memset (dest_p + remaining_length, 0, sub_length);
						}
				}
			else
				{
					#if BYTE_BUFFER_DEBUG >= 0
					DB (KPRINTF ("%s %ld - extract failure: sub_length \"%lu\"\n", __FILE__, __LINE__,sub_length));
					DebugPrintByteBuffer (buffer_p);
					#endif
				}
		}

	#if BYTE_BUFFER_DEBUG >= 10
	DB (KPRINTF ("%s %ld - ********** post-extract\n", __FILE__, __LINE__));
	DebugPrintByteBuffer (buffer_p);
	#endif


	LEAVE ();

	return substring_s;
}


void DebugPrintByteBuffer (const ByteBuffer * const buffer_p)
{
	ENTER ();

	DB (KPRINTF ("%s %ld - ByteBuffer: size %lu index %lu data \n\"%s\" %.8X (%ld) %.8X\n", __FILE__, __LINE__, buffer_p -> bb_size,
		buffer_p -> bb_current_index, buffer_p -> bb_data_p, buffer_p -> bb_data_p, * (buffer_p -> bb_data_p + 1), (buffer_p -> bb_data_p) + (buffer_p -> bb_current_index)));

	LEAVE ();
}
