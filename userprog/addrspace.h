// addrspace.h 
//	Data structures to keep track of executing user programs 
//	(address spaces).
//
//	For now, we don't keep any information about address spaces.
//	The user level CPU state is saved and restored in the thread
//	executing the user program (see thread.h).
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"
#include "translate.h"
#include "machine.h"

#define UserStackSize		1024 	// increase this as necessary!

class AddrSpace {
  public:
    AddrSpace();			// Create an address space.
    ~AddrSpace();			// De-allocate an address space

    bool Load(char *fileName);		// Load a program into addr space from
                                        // a file
					// return false if not found

    void Execute();             	// Run a program
					// assumes the program has already
                                        // been loaded

    void SaveState();			// Save/restore address space-specific
    void RestoreState();		// info on a context switch 

    //Added functionality here:
    bool CreateFile(char *fn);
    int OpenReadWriteFile(char *fn);
    int ReadOpenFile(int fd, int svaddr, int size);
    int WriteOpenFile(int fd, int svaddr, int size);
    void CloseOpenFile(int fd);
    int ReadConsole(int b, int size);
    int WriteConsole(int b, int size);
    bool TlbFault(int vaddr);

    // Translate virtual address _vaddr_
    // to physical address _paddr_.
    // _writing_ is false for Read, true for Write.
    ExceptionType Translate(int vaddr, int *paddr, bool writing);

  private:
    TranslationEntry *pageTable;	// Assume linear page table translation
					// for now!
    int numPages;         		// Number of pages in the virtual 
					// address space

    void InitRegisters();		// Initialize user-level CPU registers,
					// before jumping to user code

};

#endif // ADDRSPACE_H
