/* * * * * * *
 * Modifications made by: Ammar Ahmed
 * Username: ammara
 * (Procured from a friend doing COMP20007 this semester)
 * 
 * Module for creating and manipulating singly-linked lists of integers
 *
 * created for COMP20007 Design of Algorithms 2017
 * by Matt Farrugia <matt.farrugia@unimelb.edu.au>
 */

// 'include guards': these lines (and #endif at the bottom) prevent this file
// from being included multiple times. multiple inclusion can cause errors on
// some compilers, especially if the included file contains struct definitions
// how it works: https://en.wikipedia.org/wiki/Include_guard
#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

typedef struct list List;

typedef struct node Node;

// a list node points to the next node in the list, and to some data
struct node
{
	Node* next;
	Node* back;
    void* data;
};

// a list points to its first and last nodes, and stores its size (num. nodes)
struct list
{
	Node *head;
	Node *last;
	int size;
};

// create a new list and return its pointer
List *new_list();

// destroy a list and free its memory
void free_list(List *list);

// add an element to the front of a list
// this operation is O(1)
void list_add_start(List *list, void* data);

// add an element to the back of a list
// this operation is O(1)
void list_add_end(List *list, void* data);

// remove and return the front data element from a list
// this operation is O(1)
// error if the list is empty (so first ensure list_size() > 0)
void* list_remove_start(List *list);

// remove and return the final data element in a list
// this operation is O(n), where n is the number of elements in the list
// error if the list is empty (so first ensure list_size() > 0)
void* list_remove_end(List *list);

// return the number of elements contained in a list
int list_size(List *list);

// returns whether the list contains no elements (true) or some elements (false)
bool list_is_empty(List *list);

// removes the node from the list given a PID
void list_remove_process (List* list, int pid);

#endif