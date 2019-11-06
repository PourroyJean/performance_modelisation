/**
    Thanks to http://www.jonathanbeard.io/tutorials/Mixed_C_C++
 */
#include <stdint.h>


#ifdef __cplusplus

#include <string>
using namespace std;


#endif




#ifdef __cplusplus
extern "C" 
{
#endif

int yamb_annotate_set_event (const char * event_name,const char * color);

#ifdef __cplusplus
}
#endif

