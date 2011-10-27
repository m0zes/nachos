//******************************************************************************
//
//  Name:       table.tpl
//
//  Purpose:    Template file for the Table class.
//
//******************************************************************************
//  Copyright (c) 2004, Dominic Gelinas.  All Rights Reserved.
//******************************************************************************
//
//              mm/dd/yy  fml  Comment
//  Revisions:  09/27/04  d g  Created.
//
//******************************************************************************

#ifndef TABLE_TPL
#define TABLE_TPL

//******************************************************************************
//
//  INCLUDES
//
//******************************************************************************

#include "table.h"

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
//
//              mm/dd/yy  fml  Comment
//  Revisions:  09/27/04  d g  Created.
//
//******************************************************************************
template< class T >
Table< T >::
Table(
    int base
)
    : Current_Key( base )
{
}


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
//
//              mm/dd/yy  fml  Comment
//  Revisions:  09/27/04  d g  Created.
//
//******************************************************************************
template< class T >
Table< T >::
~Table( void )
{
}


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
//
//              mm/dd/yy  fml  Comment
//  Revisions:  09/27/04  d g  Created.
//
//******************************************************************************
template< class T >
bool
Table< T >::
Extract(
    int key,
    T& item
)
{
    bool results = false ;
//
//  Find the item.
//
    Table_Map_Iter_t iter = this->Table_Map.find( key ) ;
//
//  If the item was found, remove it and return it.
//
    if  ( iter != this->Table_Map.end() ) {
        item = iter->second ;
        this->Table_Map.erase( iter ) ;
        results = true ;
    }
//
//  Return the results.
//
    return ( results ) ;
}


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
//
//              mm/dd/yy  fml  Comment
//  Revisions:  09/27/04  d g  Created.
//
//******************************************************************************
template< class T >
bool
Table< T >::
Insert(
    int& key, 
    const T& item
)
{
//
//  Create a pair, and insert it into the map.
//
    std::pair< Table_Map_Iter_t, bool > results =
        this->Table_Map.insert( Table_Map_Pair_t( this->Current_Key, item ) ) ;
//
//  If the insertion was successful, increment the current key.
//
    if  ( results.second ) {
        key = this->Current_Key ;
        this->Current_Key++ ;
    }
//
//  Return the results.
//
    return ( results.second ) ;
}


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
//
//              mm/dd/yy  fml  Comment
//  Revisions:  09/27/04  d g  Created.
//
//******************************************************************************
template< class T >
bool
Table< T >::
Is_Empty( void ) const
{
    return ( this->Table_Map.empty() ) ;
}


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
//
//              mm/dd/yy  fml  Comment
//  Revisions:  09/27/04  d g  Created.
//
//******************************************************************************
template< class T >
bool
Table< T >::
Lookup(
    int key,
    T& item
) const
{
    bool results = false ;
//
//  Find the item.
//
    Table_Map_Const_Iter_t iter = this->Table_Map.find( key ) ;
//
//  If the item was found, return it.
//
    if  ( iter != this->Table_Map.end() ) {
        item = iter->second ;
        results = true ;
    }
//
//  Return the results.
//
    return ( results ) ;
}


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
//
//              mm/dd/yy  fml  Comment
//  Revisions:  09/27/04  d g  Created.
//
//******************************************************************************
template< class T >
bool
Table< T >::
Remove(
    int& key,
    T& item
)
{
    bool results = false ;
//
//  Find the first item.
//
    Table_Map_Iter_t iter = this->Table_Map.begin() ;
//
//  If the item was found, remove it and return it.
//
    if  ( iter != this->Table_Map.end() ) {
        key = iter->first ;
        item = iter->second ;
        this->Table_Map.erase( iter ) ;
        results = true ;
    }
//
//  Return the results.
//
    return ( results ) ;
}

#endif // TABLE_TPL
