#ifndef _P3_ERROR_H_
#define _P3_ERROR_H_

#include <stdio.h>
#include <stdlib.h>
#include "debug.h"

#define NI() \
	do { fprintf(stderr, "%s is not implemented at %s::%d\n", __PRETTY_FUNCTION__, __FILE__, __LINE__); abort(); } while(0)

#endif
