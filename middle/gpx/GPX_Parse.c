#include "GPX_Parse.h"
#include "string.h"
#include "stdlib.h"
// #include "nrf_queue.h"
#include "nrf_log.h"

// NRF_QUEUE_DEF(float, gpx_queue, 128, NRF_QUEUE_MODE_NO_OVERFLOW);

char gpx_parse_buffer[1024];

int8_t gpx_parse(char *pdata, uint32_t len, float *lat, float *lon)
{
    char *p1 = pdata, *p2;
    if (strstr(p1, "<trkpt") == NULL)
    {
        return -1;
    }
    p2 = strstr(p1, "lat=");
    if (p2 != NULL)
    {
        *lat = atof(p2 + 5);
    }
    p2 = strstr(p1, "lon=");
    if (p2 != NULL)
    {
        *lon = atof(p2 + 5);
    }
    return 0;
}
