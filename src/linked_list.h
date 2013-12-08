/** \file linked_list.h
 *  \brief A doubly-linked list.
 *
 *  A doubly-linked list that is the base class for many containers within the program.
 */
#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "types.h"

#ifdef __cplusplus
	extern "C" {
#endif


/******** FORWARD DECLARATION ******/
struct Node;

/**
 * A doubly-linked node that points to
 * its predecessor and successor.
 */
typedef struct Node
{
	struct Node *ln_prev_p;		/**< A pointer to the previous Node */
	struct Node *ln_next_p;		/**< A pointer to the next Node */
} Node;


/**
 * A doubly-linked list that can be traversed in
 * either direction.
 */
typedef struct List
{
	/** A pointer to the first Node on this list */
  Node *ll_head_p;

  /** A pointer to the last Node on this list */
  Node *ll_tail_p;

  /** The number of Nodes on this list. */
  uint32 ll_size;

  /** Callback function to use when freeing a Node */
  void (*ll_free_node_fn_p) (Node * node_p);

} List;



/**
 * Create a new List.
 * The new List is initialised as an empty list.
 *
 * @param free_node_fn_p The callback function to use when
 * freeing Nodes. If this isn't set the default is to
 * call free ().
 * @return The new List or NULL upon error.
 */
List *AllocateList (void (*free_node_fn_p) (Node * const node_p));


/**
 * Free a List.
 * Each node is passed to the list's ll_free_node_fn_p or
 * stdlib's free if ll_free_node_fn_p is NULL to be freed.
 * Then the List is also freed
 *
 * @param list_p The List to free.
 */
 void FreeList (List * const list_p);


/**
 * Initialise/clear a List to be empty.
 *
 * @param list_p The List to initialise.
 */
 void InitList (List * const list_p);


struct Node *ListGetHead (List * const list_p);
struct Node *ListGetTail (List * const list_p);
struct Node *ListGetNext (Node * const node_p);

/**
 * Add a Node to the start of a List.
 *
 * @param list_p The List to add the Node to.
 * @param node_p The Node to add.
 */
 void ListAddHead (List * const list_p, Node * const node_p);


/**
 * Add a Node to the end of a List.
 *
 * @param list_p The List to add the Node to.
 * @param node_p The Node to add.
 */
 void ListAddTail (List * const list_p, Node * const node_p);


/**
 * Remove the first Node from a List.
 *
 * @param list_p The List to remove the first Node from.
 * @return The removed Node or NULL if the List is empty.
 */
 Node *ListRemHead (List * const list_p);


/**
 * Remove the last Node from a List.
 *
 * @param list_p The List to remove the last Node from.
 * @return The removed Node or NULL if the List is empty.
 */
 Node *ListRemTail (List * const list_p);


/**
 * Remove a Node from a List.
 *
 * @param list_p The List to remove the Node from.
 * @param node_p The Node to be removed.
 */
 void ListRemove (List * const list_p, Node * const node_p);


/**
 * Sort a List.
 *
 * @param list_p The List to sort.
 * @param compare_nodes_fn Function used to sort the Nodes.
 * @return
 * It returns < 0 if node1_p should be before node2_p,
 * It returns > 0 if node1_p should be after node2_p,
 * It returns  0 if node1_p is equal to node2_p.
 */
 BOOL ListSort (List * const list_p, int (*compare_nodes_fn) (const void *v1_p, const void *v2_p));


/**
 * Set the function used to free ListsNodes when the List is freed.
 *
 * @param list_p The List to set the function for.
 * @param free_node_fn The function that will be used to free the Nodes.
 */
 void SetListFreeNodeFunction (List * const list_p, void (*free_node_fn) (Node * const node_p));


/**
 * Insert a node into a list using the given sorting algorithm
 * to determine the node's position on the list.
 *
 * @param list_p The List to insert the node on.
 * @param node_p The Node to insert into the list.
 * @param compare_nodes_fn Function used to compare the Nodes.
 */
 void ListPrioritisedInsert (List * const list_p, Node * const node_p, int (*compare_nodes_fn) (const void *v1_p, const void *v2_p));


/**
 * Insert a node into a list after a given node.
 *
 * @param list_p The List to insert the node on.
 * @param prev_node_p The Node to insert our new node directly after. If this NULL, then our node will
 * be inserted at the start of the list.
 * @param node_to_insert_p The Node to insert on the list.
 */
 void ListInsert (List * const list_p, Node * const prev_node_p, Node * const node_to_insert_p);


/**
 * Search a sorted list using the given sorting algorithm
 * for a given node
 *
 * @param list_p The List to search.
 * @param node_p The Node to search for.
 * @param compare_nodes_fn Function used to compare the Nodes.
 * @return The matching Node or NULL if it could not be found on the List.
 */
 Node *ListBinarySearch (const List * const list_p, const Node * const node_p, int (*compare_nodes_fn) (const void *v1_p, const void *v2_p), int * const index_p);


/**
 * Move the contents of one list to another.
 * This function is the equivalent of a "cut and paste" of all of the nodes from one list
 * to another. The nodes are appended to the end of the destination list in the same order in
 * which they appeared in the source list which is then left empty.
 *
 * @param src_list_p The List to remove all of the nodes from.
 * @param dest_list_p The List to which the nodes will be appended.
 */
 void MoveListContents (List * const src_list_p, List * const dest_list_p);


/**
 * Cut a list into two lists at the given index.
 *
 * @param list_p The List to split.
 * @param split_list_head_index The index of the Node that will be the first Node
 * on the newly created list, i.e. its ll_head_p.
 * @return The newly created list or NULL if there was an error or if split_list_head_index
 * was not less than the length of list_p.
 */
 List *SplitList (List * const list_p, const uint32 split_list_head_index);


/**
 * Empty a List by freeing all of its Nodes.
 *
 * @param list_p The List to empty.
 */
 void ClearList (List * const list_p);


#ifdef __cplusplus
}
#endif

#endif	/* LINKED_LIST_H */


