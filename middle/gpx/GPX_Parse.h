#ifndef _GPX_PARSE_H_
#define _GPX_PARSE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

int8_t gpx_parse(char *pdata, uint32_t len, float *lat, float *lon);

#ifdef __cplusplus
}
#endif

#endif
