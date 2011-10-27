// thread_exception_handlers.cc 
//	sample code for thread-related system calls

#include "copyright.h"
#include "system.h"
#include "syscall.h"

#if defined(CHANGED) && defined(USER_PROGRAM)
void ExceptionExit(int n) {
  printf("Exit(%d)\n", n);
  currentThread->Exit(n, systemLock);
  systemLock->Release();
  currentThread->Finish();
  // not reached 
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

}
