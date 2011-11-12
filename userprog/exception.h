// exception.cc 
//    Entry point into the Nachos kernel from user programs.
//    There are two kinds of things that can cause control to
//    transfer back to here from user code:
//
//    syscall -- The user code explicitly requests to call a procedure
//    in the Nachos kernel.  Right now, the only function we support is
//    "Halt".
//
//    exceptions -- The user code does something that the CPU can't handle.
//    For instance, accessing memory that doesn't exist, arithmetic errors,
//    etc.  
//
//    Interrupts (which can also cause control to transfer from user
//    code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef EXCEPTION_H
#define EXCEPTION_H
#include "copyright.h"
void WriteChar(char c, int vaddr);
void WriteString(int vaddr, char* buff, int buffsize);
void WriteBuffer(int length, int vaddr, char* buff);
char ReadChar(int vaddr);
int ReadString(int vaddr, char* buff, int buffsize);
#endif
