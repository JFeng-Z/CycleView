#include "GPX_Link.h"
#include "GPX.h"
#include "WString.h"
#include "string.h"
#include "nrf_log.h"

// 创建 GPX 类的实例
static GPX gpx;
static char cached_str[512];

void gpx_setMetaName(char *name)
{
    gpx.setMetaName(name);
}

void gpx_setMetaDesc(char *desc)
{
    gpx.setMetaDesc(desc);
}

void gpx_setName(char *name)
{
    gpx.setName(name);
}

void gpx_setDesc(char *desc)
{
    gpx.setDesc(desc);
}

void gpx_setEle(char *ele)
{
    gpx.setEle(ele);
}

void gpx_setSym(char *sym)
{
    gpx.setSym(sym);
}

void gpx_setSpeed(char *speed)
{
    gpx.setSpeed(speed);
}

void gpx_setTime(char *time)
{
    gpx.setTime(time);
}

void gpx_setSrc(char *src)
{
    gpx.setSrc(src);
}

char *gpx_getOpen(void)
{
    memset(cached_str, 0, sizeof(cached_str));
    strcpy(cached_str, gpx.getOpen().c_str());
    return cached_str;
}

char *gpx_getMetaData(void)
{
    memset(cached_str, 0, sizeof(cached_str));
    strcpy(cached_str, gpx.getMetaData().c_str());
    return cached_str;
}

char *gpx_getTrakOpen(void)
{
    memset(cached_str, 0, sizeof(cached_str));
    strcpy(cached_str, gpx.getTrakOpen().c_str());
    return cached_str;
}

char *gpx_getInfo(void)
{
    memset(cached_str, 0, sizeof(cached_str));
    strcpy(cached_str, gpx.getInfo().c_str());
    return cached_str;
}

char *gpx_getTrakSegOpen(void)
{
    memset(cached_str, 0, sizeof(cached_str));
    strcpy(cached_str, gpx.getTrakSegOpen().c_str());
    return (char *)cached_str;
}

char *gpx_getPt(char *lat, char *lon)
{
    memset(cached_str, 0, sizeof(cached_str));
    strcpy(cached_str, gpx.getPt(GPX_TRKPT, lon, lat).c_str());
    return cached_str;
}

char* gpx_getTrakSegClose(void) 
{
    memset(cached_str, 0, sizeof(cached_str));
    strcpy(cached_str, gpx.getTrakSegClose().c_str());
    return cached_str;
}

char *gpx_getTrakClose(void)
{
    memset(cached_str, 0, sizeof(cached_str));
    strcpy(cached_str, gpx.getTrakClose().c_str());
    return cached_str;
}

char *gpx_getClose(void)
{
    memset(cached_str, 0, sizeof(cached_str));
    strcpy(cached_str, gpx.getClose().c_str());
    return cached_str;
}