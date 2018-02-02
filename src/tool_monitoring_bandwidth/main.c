#include <stdlib.h>
#include "code_annotation.h"
#include <unistd.h>


int main(const int argc, const char **argv)
{
   


  sleep (1);
  yamb_annotate_set_event("Flag1second", "red");
  sleep (0.5);
  yamb_annotate_set_event("Flag2second", "blue");
  sleep (1);
  yamb_annotate_set_event("Flag3second", "orange");
  sleep (0.5);
  yamb_annotate_set_event ("string event_name", "yellow");

   return( EXIT_SUCCESS );
}
