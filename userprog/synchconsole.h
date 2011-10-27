// synchconsole.h 
// 	Data structures to export a synchronous interface to the raw 
//	console device.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#ifndef SYNCHCONSOLE_H
#define SYNCHCONSOLE_H

#include "console.h"
#include "synch.h"
#if defined(CHANGED) && defined(USER_PROGRAM)

// The following class defines a "synchronous" console abstraction.
// As with other I/O devices, the raw physical console is an asynchronous device --
// requests to read or write portions of the console return immediately,
// and an interrupt occurs later to signal that the operation completed.
// (Also, the physical characteristics of the console device assume that
// only one operation can be requested at a time).
//
// This class provides the abstraction that for any individual thread
// making a request, it waits around until the operation finishes before
// returning.
class SynchConsole {
  public:
    SynchConsole(char* name, char *in, char *out);
    ~SynchConsole();		
    
    void PutChar(char data);
    char GetChar();
    void GetCharReady();
    void PutCharDone();
		
  private:
    Console *console;		  		// Raw console device
    Semaphore *putCharSemaphore;
    Semaphore *getCharSemaphore;
    Lock *putCharLock;
    Lock *getCharLock;
};

#endif // SYNCHCONSOLE_H
#endif
