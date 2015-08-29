#include "list_utils.h"


void FreeList (struct List *list_p)
{
	struct Node *node_p;

	for (node_p = IExec->GetHead (list_p); node_p != NULL; node_p = IExec->GetSucc (node_p))
		{
			if (node_p -> ln_Name)
				{
					IExec->FreeVec (node_p -> ln_Name);
				}

			IExec->FreeSysObject (ASOT_NODE, node_p);
		}

	IExec->FreeSysObject (ASOT_LIST, (APTR) filenames_p);
}


uint32 GetListSize (struct List * const list_p)
{
	uint32 i = 0;
	struct Node *node_p;

	for (node_p = IExec->GetHead (list_p); node_p != NULL; node_p = IExec->GetSucc (node_p))
		{
			++ i;

			if (node_p -> ln_Name)
				{
					DB (KPRINTF ("%s %ld - GetListSize i: %lu node name %s\n", __FILE__, __LINE__, i, node_p -> ln_Name));
				}
			else
				{
					DB (KPRINTF ("%s %ld - GetListSize i: %lu node name <NULL>\n", __FILE__, __LINE__, i));
				}
		}

	return i;
}

BOOL SortList (struct List *list_p, int (*compare_fn) (const void *v0_p, const void *v1_p))
{
	BOOL success_flag = FALSE;
	const uint32 size = GetListSize (list_p);
	struct Node **nodes_pp = IExec->AllocVecTags (size * sizeof (Node *), TAG_DONE);

	if (nodes_pp)
		{
			uint32 i;
			struct Node **node_pp = nodes_pp;

			for (i = size; i > 0; -- i, ++ node_pp)
				{
					*node_pp = IExec->RemHead (list_p);
				}

			qsort (nodes_pp, size, sizeof (Node *), compare_fn);

			for (i = size, node_pp = nodes_pp; i > 0; -- i, ++ node_pp)
				{
					IExec->AddTail (list_p, *node_pp);
				}

			IExec->FreeVec (nodes_pp);
			success_flag = TRUE;
		}

	return success_flag;
}