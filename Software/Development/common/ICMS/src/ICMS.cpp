

#include "ICMS.h"
#include <memory>


#define ICMS_MEM __attribute__((section (".ICMS_section")))


ICMS& ICMS::Instance()
{
    static ICMS_MEM ICMS instance;
    return instance;
}