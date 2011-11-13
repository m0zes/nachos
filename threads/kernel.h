// kernel.h
//	Global variables for the Nachos kernel.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef KERNEL_H
#define KERNEL_H

#include "copyright.h"
#include "debug.h"
#include "utility.h"
#include "thread.h"
#include "scheduler.h"
#include "interrupt.h"
#include "stats.h"
#include "alarm.h"
#include "filesys.h"
#include "machine.h"
#include "synchconsole.h"

class PostOfficeInput;
class PostOfficeOutput;
//class SynchConsoleInput : public SynchConsole {
//	public:
//		SynchConsoleInput(char* in);
//		~SynchConsoleInput();
//		char GetChar();
//};
//class SynchConsoleOutput : public SynchConsole {
//	public:
//		SynchConsoleOutput(char* out);
//		~SynchConsoleOutput();
//		void PutChar(char c);
//
//};
class SynchDisk;

class Kernel {
  public:
    Kernel(int argc, char **argv);
    				// Interpret command line arguments
    ~Kernel();		        // deallocate the kernel
    
    void Initialize(); 		// initialize the kernel -- separated
				// from constructor because 
				// refers to "kernel" as a global

    void Run();                 // do kernel stuff

    void ThreadSelfTest();	// self test of threads and synchronization

    void ConsoleTest();         // interactive console self test

    void NetworkTest();         // interactive 2-machine network test
    
// These are public for notational convenience; really, 
// they're global variables used everywhere.

    Thread *currentThread;	// the thread holding the CPU
    Scheduler *scheduler;	// the ready list
    Interrupt *interrupt;	// interrupt status
    Statistics *stats;		// performance metrics
    Alarm *alarm;		// the software alarm clock    
    Machine *machine;           // the simulated CPU
    SynchConsole *synchConsoleIn;
    SynchConsole *synchConsoleOut;
    SynchDisk *synchDisk;
    FileSystem *fileSystem;     
#ifdef NETWORK
    PostOfficeInput *postOfficeIn;
    PostOfficeOutput *postOfficeOut;
#endif

    int hostName;               // machine identifier

  private:
    bool randomSlice;		// enable pseudo-random time slicing
    bool debugUserProg;         // single step user program
    double reliability;         // likelihood messages are dropped
    char *consoleIn;            // file to read console input from
    char *consoleOut;           // file to send console output to
#ifndef FILESYS_STUB
    bool formatFlag;          // format the disk if this is true
#endif
};


#endif // KERNEL_H


