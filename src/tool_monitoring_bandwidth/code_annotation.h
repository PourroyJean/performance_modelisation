/**
    Thanks to http://www.jonathanbeard.io/tutorials/Mixed_C_C++
 */
#include <stdint.h>


#ifdef __cplusplus

#include <string>
using namespace std;


#endif

#define YAMB_ANNOTATE_LOG_FILE "/nfs/pourroy/tmp/yamb_annotate_log_file"



#ifdef __cplusplus
extern "C" 
{
#endif

int yamb_annotate_set_event (char * event_name, char * color);

#ifdef __cplusplus
}
#endif

