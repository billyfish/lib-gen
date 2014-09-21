#include <proto/exec.h>

#include "string_list.h"


BOOL AddStringToStringList (struct List *list_p, STRPTR str_p, const MEM_FLAG mem_flag)
{
	BOOL success_flag = FALSE;
	StringNode *node_p = AllocateStringNode (str_p, mem_flag);

	if (node_p)
		{
			IExec->AddTail (list_p, (struct Node *) node_p);
			success_flag = TRUE;
		}

	return success_flag;
}


StringListNode *AllocateStringNode (STRPTR str_p, const MEM_FLAG mem_flag)
{
	StringNode *node_p = (StringNode *) IExec->AllocVecTags (sizeof (StringListNode), TAG_END);

	if (node_p)
		{
			STRPTR dest_p = NULL;

			switch (mem_flag)
				{
					case MF_DEEP_COPY:
						dest_p = strdup (str_p);

						if (dest_p)
							{
								node_p -> sln_string_p = dest_p;
							}
						else
							{
								IExec->FreeVec (node_p);
								node_p = NULL;
							}
						break;

					case MF_SHALLOW_COPY:
					case MF_SHADOW_USE:
						node_p -> sln_string_p = STRPTR str_p;
						break;
				}

			if (node_p)
				{
					node_p -> sln_string_flag = mem_flag;
				}
		}

	return node_p;
}


void FreeStringNode (ListNode * const node_p)
{
	StringNode * const str_node_p = (StringNode * const) node_p;

	switch (str_node_p -> sln_string_flag)
		{
			case MF_DEEP_COPY:
			case MF_SHALLOW_COPY:
				IExec->FreeVec (str_node_p -> sln_string_p);
				break;

			case MF_SHADOW_USE:
			case MF_ALREADY_FREED:
			default:
				break;
		}

	IExec->FreeVec (str_node_p);
}
