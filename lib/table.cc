// table.cc 
//
//     	Routines to manage a singly-linked table of "things".
//
// 	A "TableElement" is allocated for each item to be put on the
//	table; it is de-allocated when the item is removed. This means
//      we don't need to keep a "next" pointer in every object we
//      want to put on a table.
// 
//     	NOTE: Mutual exclusion must be provided by the caller.
//  	If you want a synchronized table, you must use the routines 
//	in synchtable.cc.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#if defined(CHANGED) && defined(USER_PROGRAM)
#include "copyright.h"
#include "table.h"

//----------------------------------------------------------------------
// TableElement::TableElement
// 	Initialize a table element, so it can be added somewhere on a table.
//
//	"itemPtr" is the item to be put on the table.  It can be a pointer
//		to anything.
//	"sortKey" is the priority of the item, if any.
//----------------------------------------------------------------------

TableElement::TableElement(void *itemPtr, int sortKey)
{
     item = itemPtr;
     key = sortKey;
     next = NULL;	// assume we'll put it at the end of the table 
}

//----------------------------------------------------------------------
// Table::Table
//	Initialize a table, empty to start with.
//	Elements can now be added to the table.
//----------------------------------------------------------------------

Table::Table()
{ 
    first = last = NULL; 
    currentKey = 2;
}

//----------------------------------------------------------------------
// Table::~Table
//	Prepare a table for deallocation.  If the table still contains any 
//	TableElements, de-allocate them.  However, note that we do *not*
//	de-allocate the "items" on the table -- this module allocates
//	and de-allocates the TableElements to keep track of each item,
//	but a given item may be on multiple tables, so we can't
//	de-allocate them here.
//----------------------------------------------------------------------

Table::~Table()
{ 
    while (this->Remove(NULL) != NULL)
	;	 // delete all the table elements
}

//----------------------------------------------------------------------
// Table::Lookup
// 
// Returns:
//	Pointer to lookuped item, NULL not there
//----------------------------------------------------------------------
void *
Table::Lookup(int k)
{
    TableElement *ptr;		// keep track

    for (ptr = first; ptr != NULL; ptr = ptr->next) {
      if (ptr->key == k) { 
        return(ptr->item);
      }
    }
    return(NULL);
}

//----------------------------------------------------------------------
// Table::Extract
// 
// Returns:
//	Pointer to extracted item, NULL if not there
//----------------------------------------------------------------------
void *
Table::Extract(int k)
{
    TableElement *ptr;	
    TableElement *prev;	
    void *ret = NULL;

    if (first->key == k) { 
      ptr = first;
      ret = ptr->item;      
      first = ptr->next;
      if (first == NULL) { 
        last = NULL;
      }
      delete ptr;
    } else if (first->next != NULL) { 
      for (prev = first, ptr = first->next;
           ptr != NULL; 
           ptr = ptr->next, prev = prev->next) {
        if (ptr->key == k) { 
          ret = ptr->item;
          prev->next = ptr->next;
          if (ptr == last) { 
            last = prev;
          }
          delete ptr;
        }
      }
    }
    return(ret);
}

//----------------------------------------------------------------------
// Table::Remove
//      Remove the first "item" from the front of the table.
// 
// Returns:
//	Pointer to removed item, NULL if nothing on the table.
//----------------------------------------------------------------------

void *
Table::Remove(int *keyPtr)
{
    TableElement *element = first;
    void *thing;

    if (IsEmpty()) 
	return NULL;

    thing = first->item;
    if (first == last) {	// table had one item, now has none 
        first = NULL;
	last = NULL;
    } else {
        first = element->next;
    }
    if (keyPtr != NULL)
        *keyPtr = element->key;
    delete element;
    return thing;
}

//----------------------------------------------------------------------
// Table::IsEmpty
//      Returns TRUE if the table is empty (has no items).
//----------------------------------------------------------------------

bool
Table::IsEmpty() 
{ 
    if (first == NULL)
        return TRUE;
    else
	return FALSE; 
}

//----------------------------------------------------------------------
// Table::Insert
//      Insert an "item" into the table.
//
//      Allocate a TableElement to keep track of the item.
//      If the table is empty, then this will be the only element.
//      Otherwise, put it at the end.
//
//      "item" is the thing to put on the table, it can be a pointer to
//              anything.
//----------------------------------------------------------------------

int
Table::Insert(void *item)
{
    int k; 
    k = currentKey;
    currentKey++;
    TableElement *element = new TableElement(item, k);
 
    if (IsEmpty()) {            // table is empty
        first = element;
        last = element;
    } else {                    // else put it after last
        last->next = element;
        last = element;
    }
    return(k);
}
#endif
