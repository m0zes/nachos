// synchconsole.cc 
//	Routines to synchronously access the console.  The physical console 
//	is an asynchronous device (console requests return immediately, and
//	an interrupt happens later on).  This is a layer on top of
//	the console providing a synchronous interface (requests wait until
//	the request completes).
//
//	Use a semaphore to synchronize the interrupt handlers with the
//	pending requests.  And, because the physical console can only
//	handle one operation at a time, use a lock to enforce mutual
//	exclusion.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synchconsole.h"
#include "callback.h"

//#if defined(CHANGED) && defined(USER_PROGRAM)

static void
getCharReady(int arg)
{
    SynchConsole* cns = (SynchConsole *)arg;

    cns->GetCharReady();
}

static void
putCharDone(int arg)
{
    SynchConsole* cns = (SynchConsole *)arg;

    cns->PutCharDone();
}

//----------------------------------------------------------------------
// SynchConsole::SynchConsole
// 	Initialize the synchronous interface to the physical console, in turn
//	initializing the physical console.
//
//	"name" -- UNIX file name to be used as storage for the console data
//	   (usually, "DISK")
//----------------------------------------------------------------------

SynchConsole::SynchConsole(char* name, char *in, char *out)
{
    putCharSemaphore = new Semaphore("synch console put char", 0);
    getCharSemaphore = new Semaphore("synch console get char", 0);
    putCharLock = new Lock("synch console put char lock");
    getCharLock = new Lock("synch console get char lock");
    //consolein = new ConsoleInput(in, getCharReady, (int) this);
    consolein = new ConsoleInput(in, (CallBackObj*)getCharReady);
    //consoleout = new ConsoleOutput(out, putCharDone, (int) this);
    consoleout = new ConsoleOutput(out, (CallBackObj*)putCharDone);
}

//----------------------------------------------------------------------
// SynchConsole::~SynchConsole
// 	De-allocate data structures needed for the synchronous console
//	abstraction.
//----------------------------------------------------------------------

SynchConsole::~SynchConsole()
{
    delete consolein;
    delete consoleout;
    delete putCharSemaphore;
    delete getCharSemaphore;
    delete putCharLock;
    delete getCharLock;
}

char
SynchConsole::GetChar() {
  char ret;
  getCharLock->Acquire();
  getCharSemaphore->P();
  ret = consolein->GetChar();
  getCharLock->Release();
  return(ret);
}

void 
SynchConsole::GetCharReady() {
  getCharSemaphore->V();
}

void
SynchConsole::PutChar(char ch) {
  putCharLock->Acquire();
  consoleout->PutChar(ch);
  putCharSemaphore->P();
  putCharLock->Release();
}

void 
SynchConsole::PutCharDone() {
  putCharSemaphore->V();
}
//#endif
