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

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "system.h"
#include "exception.h"
#include "machine.h"
#include "kernel.h"

void WriteChar(char c, int vaddr) {
    int phyAddr;
    //cout << "kernel->currentThread->space = " << (int) kernel->currentThread->space << endl;
    kernel->currentThread->space->Translate(vaddr, &phyAddr, TRUE);
    kernel->machine->mainMemory[phyAddr] = c;
}

void WriteString(int vaddr, char* buff, int buffsize) {
    int i;
    for (i = 0; i < buffsize; i++) {
        WriteChar(buff[i], vaddr + i);
        if (buff[i] == '\0')
            break;
    }
}

void WriteBuffer(int length, int vaddr, char* buff) {
    int i;
    for (i = 0; i < length; i++) {
        WriteChar(buff[i], vaddr + i);
    }
}

char ReadChar(int vaddr) {
   int phyAddr;
   kernel->currentThread->space->Translate(vaddr, &phyAddr, FALSE);
   char c = kernel->machine->mainMemory[phyAddr];
   return c;
}

int ReadString(int vaddr, char* buff, int buffsize) {
    int size;
    for (size = 0; size < buffsize; size++) {
        buff[size] = ReadChar(vaddr + size);
        if (buff[size] == '\0')
            break;
    }
    return size;
}

void ExceptionHalt() {
    kernel->interrupt->Halt();
}

int ExceptionAdd(int op1, int op2) {
    return ( op1 + op2 );
}
//#if defined(CHANGED) && defined(USER_PROGRAM)
void ExceptionExit(int n) {
  printf("Exit(%d)\n", n);
  //currentThread->Exit(n, kernel->machine->systemLock);
  kernel->systemLock->Release();
  kernel->currentThread->Finish();
  ASSERTNOTREACHED();
}

void ForkExec(int dummy) { 
  AddrSpace *space = kernel->currentThread->space;

  //space->InitRegisters();             // set the initial register values
  //space->RestoreState();              // load page table register
 
  kernel->machine->Run();                     // jump to the user progam
  ASSERT(FALSE);                      // machine->Run never returns;
                                        // the address space exits
                                        // by doing the syscall "exit"
}
//#endif

#define SizeExceptionFilename 64

int 
ExceptionExec(int fn) { 
  Thread *t;
  bool failed;
  char filename[SizeExceptionFilename];
  int ret;
  if (!(ReadString(fn, filename, SizeExceptionFilename))) { 
    printf("Exec: Unable to read filename at address %x\n", fn);
    return(0);
  } 
  filename[SizeExceptionFilename - 1] = '\0';
  OpenFile *executable = kernel->fileSystem->Open(filename);
  AddrSpace *space;
  if (executable == NULL) { 
    printf("Exec: Unable to open file %s\n", filename);
    return(0);
  }
  space = new AddrSpace();
  //space->Load(executable);
  delete executable;
  if (failed) { 
    delete space;
    printf("Exec: Unable to read in executable for file %s\n", filename);
    return(0);
  } 
  //t = new Thread("Exec Thread", &ret);
  t->space = space;
  //t->Fork(ForkExec, 0);
  return(ret);
}

int ExceptionJoin(int id) { 
  return 0;
  //Thread *t;
  //t = (Thread *) (threadTable->Lookup(id));
  //if (t == NULL) { 
  //  return(0);
  //}
  //return(currentThread->Join(t, systemLock));
}

//#endif

int ExceptionWrite(int b, int size, int fd) {
    int ret;
    kernel->systemLock->Release();
    //DEBUG('e', "Write(%d, %d, %d)\n", b, size, fd);
    //if (fd == ConsoleOutput) {
    //    ret = currentThread->WriteConsole(b, size);
    //} else {
    //    ret = currentThread->WriteOpenFile(fd, b, size);
    //}
    kernel->systemLock->Acquire();
    return(ret);
}

int ExceptionOpen(int fn) {
    int ret;
    char filename[SizeExceptionFilename];
    printf("Open(%d)\n", fn);
    if (!(ReadString(fn, filename, SizeExceptionFilename))) {
        printf("Exec: Unable to read filename at address %x\n", fn);
        return(0);
    }
    filename[SizeExceptionFilename - 1] = '\0';
    kernel->systemLock->Release();
    // fix this! 
    //ret = currentThread->OpenReadWriteFile(filename);
    ret = 5;
    kernel->systemLock->Acquire();
    return(ret);
}

int ExceptionCreate(int fp) {
    printf("Create: Not Implemented! Trying to create %x\n", fp);
    return(0);
}

int ExceptionRead(int fp, int size, int fs) {
    printf("Read: Not Implemented! Trying to read %x\n", fp);
    return(0);
}

int ExceptionClose(int fp) {
    printf("Close: Not Implemented! Trying to close %x\n", fp);
    return(0); 
}

//----------------------------------------------------------------------
// ExceptionHandler
//     Entry point into the Nachos kernel.  Called when a user program
//    is executing, and either does a syscall, or generates an addressing
//    or arithmetic exception.
//
//     For system calls, the following is the calling convention:
//
//     system call code -- r2
//        arg1 -- r4
//        arg2 -- r5
//        arg3 -- r6
//        arg4 -- r7
//
//    The result of the system call, if any, must be put back into r2. 
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//    "which" is the kind of exception.  The list of possible exceptions 
//    is in machine.h.
//----------------------------------------------------------------------
//#define SC_Halt         0
//#define SC_Exit         1
//#define SC_Exec         2
//#define SC_Join         3
//#define SC_Create       4
//#define SC_Remove       5
//#define SC_Open         6
//#define SC_Read         7
//#define SC_Write        8
//#define SC_Seek         9
//#define SC_Close        10
//#define SC_ThreadFork   11
//#define SC_ThreadYield  12
//#define SC_ExecV        13
//#define SC_ThreadExit   14
//#define SC_ThreadJoin   15
//
//#define SC_Add          42
//

void
ExceptionHandler(ExceptionType which)
{
    int type = kernel->machine->ReadRegister(2);

    //DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

    switch (which) {
        case SyscallException:
            switch(type) {
                case SC_Halt:
                    ExceptionHalt();

                    ASSERTNOTREACHED();
                    break;

                case SC_Add:
    
                    /* Process SysAdd Systemcall*/
                    int result;
                    result = ExceptionAdd(kernel->machine->ReadRegister(4),
                                          kernel->machine->ReadRegister(5));

                    /* Prepare Result */
                    kernel->machine->WriteRegister(2, (int)result);
    
                    /* Modify return point */
                    /* set previous programm counter (debugging only)*/
                    kernel->machine->WriteRegister(
                        PrevPCReg,
                        kernel->machine->ReadRegister(PCReg));
                    
                    /* set programm counter to next instruction
                       (all Instructions are 4 byte wide)*/
                    kernel->machine->WriteRegister(
                        PCReg,
                        kernel->machine->ReadRegister(PCReg) + 4);
      
                    /* set next programm counter for brach execution */
                    kernel->machine->WriteRegister(
                        NextPCReg,
                        kernel->machine->ReadRegister(PCReg)+4);

                    return;
    
                    ASSERTNOTREACHED();

                    break;

                case SC_Exit:
                    ExceptionExit(kernel->machine->ReadRegister(4));
		    return;
                    break;

                case SC_Create:
		    {
                    int createfnpointer = kernel->machine->ReadRegister(4);
                    int createret = ExceptionCreate(createfnpointer);
                    kernel->machine->WriteRegister(2, createret);
		    return;
                    break;
		    }

                case SC_Open:
		    {
                    int openfn = kernel->machine->ReadRegister(4);
                    int openret = ExceptionOpen(openfn);
                    kernel->machine->WriteRegister(2, openret);
		    return;
                    break;
		    }

                case SC_Read:
		    {
                    int readfnpointer = kernel->machine->ReadRegister(4);
                    int readsize = kernel->machine->ReadRegister(5);
                    int readfd = kernel->machine->ReadRegister(6);
                    int readret = ExceptionRead(readfnpointer, readsize, readfd);
                    kernel->machine->WriteRegister(2, readret);
		    return;
                    break;
		    }

                case SC_Write:
		    {
                    int writeb = kernel->machine->ReadRegister(4);
                    int writesize = kernel->machine->ReadRegister(5);
                    int writefd = kernel->machine->ReadRegister(6);
                    int writeret = ExceptionWrite(writeb, writesize, writefd);
                    kernel->machine->WriteRegister(2, writeret);
		    return;
                    break;
		    }

                case SC_Close:
		    {
                    int closefd = kernel->machine->ReadRegister(4);
                    int closeret = ExceptionClose(closefd);
                    kernel->machine->WriteRegister(2, closeret);
		    return;
                    break;
		    }

                default:
                    cerr << "Unexpected system call " << type << "\n";
		    return;
                    break;
            }
            break;
        default:
            cerr << "Unexpected user mode exception" << (int)which << "\n";
            break;
    }
    ASSERTNOTREACHED();
}
