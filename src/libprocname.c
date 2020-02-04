#include <sys/prctl.h>
#include <stdlib.h>
#include <stdio.h>

static void __attribute__ ((constructor)) procname_init()
{
   const char *name;
   if ((name = getenv("PROCNAME")) && (*name)) {
      prctl(PR_SET_NAME, name);
   }
}
