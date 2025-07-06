#ifndef _WPROGRAM_H_
#define _WPROGRAM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <math.h>
#include <stdbool.h>

typedef unsigned char byte;

#define PI 3.14159265
#define TWO_PI 2*PI

#define radians(deg) deg * (PI/180);
#define degrees(rad) rad * (180/PI);
#define sq(x) ((x)*(x))

uint32_t millis(void);

#ifdef __cplusplus
}
#endif

#endif
