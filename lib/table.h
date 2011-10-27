// table.h 
//	Data structures to manage LISP-like tables.  
//
//      As in LISP, a table can contain any type of data structure
//	as an item on the table: thread control blocks, 
//	pending interrupts, etc.  That is why each item is a "void *",
//	or in other words, a "pointers to anything".
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#if defined(CHANGED) && defined(USER_PROGRAM)
#ifndef TABLE_H
#define TABLE_H

#include "copyright.h"
#include "utility.h"

// The following class defines a "table element" -- which is
// used to keep track of one item on a table.  It is equivalent to a
// LISP cell, with a "car" ("next") pointing to the next element on the table,
// and a "cdr" ("item") pointing to the item on the table.
//
// Internal data structures kept public so that Table operations can
// access them directly.

class TableElement {
   public:
     TableElement(void *itemPtr, int sortKey);	// initialize a table element

     TableElement *next;		// next element on table, 
				// NULL if this is the last
     int key;		    	// priority, for a sorted table
     void *item; 	    	// pointer to item on the table
};

// The following class defines a "table" -- a singly linked table of
// table elements, each of which points to a single item on the table.
//
// By using the "Sorted" functions, the table can be kept in sorted
// in increasing order by "key" in TableElement.

class Table {
  public:
    Table();			// initialize the table
    ~Table();			// de-allocate the table
    int Insert(void *item);
    void *Lookup(int sortKey);
    void *Extract(int sortKey);
    void *Remove(int *sortKey);
    bool IsEmpty();		// is the table empty? 

  private:
    TableElement *first;  	// Head of the table, NULL if table is empty
    TableElement *last;		// Last element of table
    int currentKey;
};

#endif // TABLE_H
#endif
