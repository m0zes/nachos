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

#define MAXBUFFSIZE = 4096

void WriteChar(char c, int vaddr) {
    int phyAddr;
    AddrSpace::Translate(vaddr, &phyAddr, TRUE);
    machine->mainMemory[phyAddr] = c;
}

void WriteString(int vaddr, char* buff) {
    int i;
    for (i = 0; i < MAXBUFFSIZE; i++) {
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
   AddrSpace::Translate(vaddr, &phyAddr, FALSE);
   char c = machine->mainMemory[phyAddr];
   return c;
}

int ReadString(int vaddr, char* buff) {
    int size;
    for (size = 0; size < MAXBUFFSIZE; size++) {
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
#if defined(CHANGED) && defined(USER_PROGRAM)
void ExceptionExit(int n) {
  printf("Exit(%d)\n", n);
  currentThread->Exit(n, systemLock);
  systemLock->Release();
  currentThread->Finish();
  ASSERTNOTREACHED();
}

void ForkExec(int dummy) { 
  AddrSpace *space = currentThread->space;

  space->InitRegisters();             // set the initial register values
  space->RestoreState();              // load page table register
 
  machine->Run();                     // jump to the user progam
  ASSERT(FALSE);                      // machine->Run never returns;
                                        // the address space exits
                                        // by doing the syscall "exit"
}

#define SizeExceptionFilename 64

int 
ExceptionExec(int fn) { 
  Thread *t;
  bool failed;
  char filename[SizeExceptionFilename];
  int ret;
  if (!(currentThread->space->ReadString(fn, filename, SizeExceptionFilename))) { 
    printf("Exec: Unable to read filename at address %x\n", fn);
    return(0);
  } 
  filename[SizeExceptionFilename - 1] = '\0';
  OpenFile *executable = fileSystem->Open(filename);
  AddrSpace *space;
  if (executable == NULL) { 
    printf("Exec: Unable to open file %s\n", filename);
    return(0);
  }
  space = new AddrSpace(executable, &failed);
  delete executable;
  if (failed) { 
    delete space;
    printf("Exec: Unable to read in executable for file %s\n", filename);
    return(0);
  } 
  t = new Thread("Exec Thread", &ret);
  t->space = space;
  t->Fork(ForkExec, 0);
  return(ret);
}

int ExceptionJoin(int id) { 
  Thread *t;
  t = (Thread *) (threadTable->Lookup(id));
  if (t == NULL) { 
    return(0);
  }
  return(currentThread->Join(t, systemLock));
}

#endif

int ExceptionWrite(int b, int size, int fd) {
    int ret;
    systemLock->Release();
    DEBUG('e', "Write(%d, %d, %d)\n", b, size, fd);
    if (fd == ConsoleOutput) {
        ret = currentThread->WriteConsole(b, size);
    } else {
        ret = currentThread->WriteOpenFile(fd, b, size);
    }
    systemLock->Acquire();
    return(ret);
}

int ExceptionOpen(int fn) {
    int ret;
    char filename[SizeExceptionFilename];
    printf("Open(%d)\n", fn);
    if (!(currentThread->space->ReadString(fn, filename, SizeExceptionFilename))) {
        printf("Exec: Unable to read filename at address %x\n", fn);
        return(0);
    }
    filename[SizeExceptionFilename - 1] = '\0';
    systemLock->Release();
    ret = currentThread->OpenReadWriteFile(filename);
    systemLock->Acquire();
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

    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

#if defined(CHANGED) && defined(USER_PROGRAM)
    switch (which) {
        case SyscallException:
            switch(type) {
                case SC_Halt:
                    DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

                    ExceptionHalt();

                    ASSERTNOTREACHED();
                    break;

                case SC_Add:
                    DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) <<
                                  " + " << kernel->machine->ReadRegister(5) <<
                                  "\n");
    
                    /* Process SysAdd Systemcall*/
                    int result;
                    result = ExceptionAdd(kernel->machine->ReadRegister(4),
                                          kernel->machine->ReadRegister(5));

                    DEBUG(dbgSys, "Add returning with " << result << "\n");

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
                    DEBUG(dbgSys, "Exit due system exit due to " << kernel->machine->ReadRegister(4) << endl);
                    ExceptionExit(kernel->machine->ReadRegister(4));
                    break;

                case SC_Create:
                    DEBUG(dbgSys, "Called Create on file: args = " << kernel->machine->ReadRegister(4) << " " << kernel->machine->ReadRegister(5) << " " << kernel->machine->ReadRegister(6) << " " << kernel->machine->ReadRegister(7) << endl);
                    int fnpointer = kernel->machine-ReadRegister(4);
                    int ret = ExceptionCreate(fnpointer);
                    kernel->machine->WriteRegister(2, ret);
                    break;

                case SC_Open:
                    DEBUG(dbgSys, "Called Open on file: args = " << kernel->machine->ReadRegister(4) << endl);
                    int fn = kernel->machine->ReadRegister(4);
                    int ret = ExceptionOpen(fn);
                    kernel->machine->WriteRegister(2, ret);
                    break;

                case SC_Read:
                    DEBUG(dbgSys, "Called Read on file: args = " << kernel->machine->ReadRegister(4) << " " << kernel->machine->ReadRegister(5) << " " << kernel->machine->ReadRegister(6) << endl);
                    int fnpointer = kernel->machine->ReadRegister(4);
                    int size = kernel->machine->ReadRegister(5);
                    int fd = kernel->machine->ReadRegister(6);
                    int ret = ExceptionRead(fnpointer, size, fs);
                    kernel->machine->WriteRegister(2, ret);
                    break;

                case SC_Write:
                    DEBUG(dbgSys, "Called Write on file: args = " << kernel->machine->ReadRegister(4) << " " << kernel->machine->ReadRegister(5) << " " << kernel->machine->ReadRegister(6) << endl);
                    int b = kernel->machine->ReadRegister(4);
                    int size = kernel->machine->ReadRegister(5);
                    int fd = kernel->machine->ReadRegister(6);
                    int ret = ExceptionWrite(b, size, fd);
                    kernel->machine->WriteRegister(2, ret);
                    break;

                case SC_Close:
                    DEBUG(dbgSys, "Called Close on file: args = " << kernel->machine->ReadRegister(4) << endl);
                    int fd = kernel->machine->ReadRegister(4);
                    int ret = ExceptionClose(fd);
                    kernel->machine->WriteRegister(2, ret);
                    break;

                default:
                    cerr << "Unexpected system call " << type << "\n";
                    break;
            }
            break;
        default:
            cerr << "Unexpected user mode exception" << (int)which << "\n";
            break;
    }
    ASSERTNOTREACHED();
}
#endif
