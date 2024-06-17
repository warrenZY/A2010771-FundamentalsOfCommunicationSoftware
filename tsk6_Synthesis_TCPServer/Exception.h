#include <setjmp.h>
#include <stdio.h>

#define TRY                      \
  do {                           \
    jmp_buf buf_state;           \
    switch (setjmp(buf_state)) { \
      case 0:
#define CATCH(x) \
  break;         \
  case x:
#define ENDTRY \
  }            \
  }            \
  while (0)
#define THROW(x) longjmp(buf_state, x)

#define EXCEPTION1 (1)
#define EXCEPTION2 (2)
#define EXCEPTION3 (3)

/*
useage:

int main() {
    TRY{
      printf("Inside Try statement \n");
      THROW(EXCEPTION2);
      printf("This does not appear as exception has already been called \n");
    }
    CATCH(EXCEPTION1) { printf("Exception 1 called \n"); }
    CATCH(EXCEPTION2) { printf("Exception 2 called \n"); }
    CATCH(EXCEPTION3) { printf("Exception 3 called \n"); }
    ENDTRY;

    return 0;
}
*/