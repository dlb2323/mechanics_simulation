#ifndef UTILS_H
#define UTILS_H

#define DEBUG
// debug output
#ifdef DEBUG
#include "stdio.h"
#define DEBUG_TEXT(message) printf("[DEBUG] :: %s\n", message);  
#define DEBUG_VARS(args...) fprintf(stdout, args);
#define DEBUG_FLOAT(message) printf("[DEBUG] :: %f\n", message);  
#else
#define DEBUG_TEXT(message)
#define DEBUG_VARS(message)
#endif

#endif // !UTILS_H
