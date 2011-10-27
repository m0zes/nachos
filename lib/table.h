//******************************************************************************
//
//  Name:       table.h
//
//  Purpose:    Header file for the Table class.
//
//******************************************************************************
//  Copyright (c) 2004, Dominic Gelinas.  All Rights Reserved.
//******************************************************************************
//
//              mm/dd/yy  fml  Comment
//  Revisions:  09/27/04  d g  Created.
//
//******************************************************************************

#ifndef TABLE_H
#define TABLE_H

//******************************************************************************
//
//  INCLUDES
//
//******************************************************************************

#include <map>

//******************************************************************************
//
//  CLASS DEFINITION
//
//******************************************************************************

//******************************************************************************
//
//  Name:       Table
//
//  Purpose:    This class is an implementation of a table that maps unique
//              integer keys to objects.
//
//******************************************************************************
template< class T >
class Table {

//******************************************************************************
//
//  TYPE DEFINITIONS
//
//******************************************************************************

 private:
    typedef std::map< int, T > Table_Map_t ;

 private:
    typedef typename Table_Map_t::const_iterator Table_Map_Const_Iter_t ;

 private:
    typedef typename Table_Map_t::iterator Table_Map_Iter_t ;
 
 private:
    typedef std::pair< int, T > Table_Map_Pair_t ;

//******************************************************************************
//
//  MEMBER FUNCTIONS
//
//******************************************************************************

//******************************************************************************
//
//  Name:       Table
//
//  Purpose:    Constructor
//
//  Parameters: base - the first key to hand out, default = 0
//
//  Returns:    Nothing
//
//******************************************************************************
 public:
    Table(
        int base = 0
    ) ;


//******************************************************************************
//
//  Name:       Table
//
//  Purpose:    Destructor
//
//  Parameters: None
//
//  Returns:    Nothing
//
//******************************************************************************
 public:
    virtual
    ~Table( void ) ;


//******************************************************************************
//
//  Name:       Extract
//
//  Purpose:    Looks up and removes an item in the table.
//
//  Parameters: key - the key to use for the lookup
//              item - the return parameter for the item found
//
//  Returns:    true  - if the key and item were found and removed
//              false - if the key and item were not found and not removed
//
//******************************************************************************
 public:
    bool
    Extract(
        int key,
        T& item
    ) ;


//******************************************************************************
//
//  Name:       Insert
//
//  Purpose:    Inserts an item in the table.
//
//  Parameters: key - the return parameter for the key paired with the item
//              item - the item to insert into the table
//
//  Returns:    true  - if the item was inserted
//              false - if the item was not inserted
//
//******************************************************************************
 public:
    bool
    Insert(
        int& key,
        const T& item
    ) ;


//******************************************************************************
//
//  Name:       Is_Empty
//
//  Purpose:    Checks if the table is empty.
//
//  Parameters: None
//
//  Returns:    true  - if the table is empty
//              false - if the table is not empty
//
//******************************************************************************
 public:
    bool
    Is_Empty( void ) const ;


//******************************************************************************
//
//  Name:       Lookup
//
//  Purpose:    Looks up an item in the table.
//
//  Parameters: key - the key to use for the lookup
//              item - the return parameter for the item found
//
//  Returns:    true  - if the key and item were found
//              false - if the key and item were not found
//
//******************************************************************************
 public:
    bool
    Lookup(
        int key,
        T& item
    ) const ;


//******************************************************************************
//
//  Name:       Remove
//
//  Purpose:    Removes the first item in the table.
//
//  Parameters: key - the return parameter for the key removed
//              item - the return parameter for the item removed
//
//  Returns:    true  - if the key and item were removed
//              false - if there were no keys and items to remove
//
//******************************************************************************
 public:
    bool
    Remove(
        int& key,
        T& item
    ) ;


//******************************************************************************
//
//  MEMBER VARIABLES
//
//******************************************************************************

//******************************************************************************
//
//  Name:       Table_Map
//
//  Purpose:    The map from keys to objects.
//
//******************************************************************************
 private:
    Table_Map_t Table_Map ;


//******************************************************************************
//
//  Name:       Current_Key
//
//  Purpose:    The next key to hand out for insertion.
//
//******************************************************************************
 private:
    int Current_Key ;


} ;

#include "table.tpl"

#endif // TABLE_H
