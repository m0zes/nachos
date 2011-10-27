// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
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

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	is in machine.h.
//----------------------------------------------------------------------

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
