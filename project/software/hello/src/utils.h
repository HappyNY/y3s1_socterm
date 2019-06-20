#include <stdio.h>

// Macro definitions
#define GPPCU_ENABLE_UNCONDITIONAL_BUBBLE_GENERATION 1

// Utilities
#define passert(cond, msg, ...) if(!(cond)) { printf("Assertion failed: %s [line %d: %s] ---> \n\t\t", #cond, __LINE__, __FILE__); printf(msg,##__VA_ARGS__ ); } 