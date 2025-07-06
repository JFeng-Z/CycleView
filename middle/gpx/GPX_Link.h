#ifndef _GPX_LINK_H_
#define _GPX_LINK_H_

#ifdef __cplusplus
extern "C" {
#endif

void gpx_setMetaName(char *name);
void gpx_setMetaDesc(char *desc);
void gpx_setName(char *name);
void gpx_setDesc(char *desc);
void gpx_setSrc(char *src);
void gpx_setEle(char *ele);
void gpx_setSym(char *sym);
void gpx_setSpeed(char *speed);
void gpx_setTime(char *time);
char *gpx_getOpen(void);
char *gpx_getMetaData(void);
char *gpx_getTrakOpen(void);
char *gpx_getInfo(void);
char *gpx_getTrakSegOpen(void);
char *gpx_getPt(char *lat, char *lon);
char *gpx_getTrakSegClose(void);
char *gpx_getTrakClose(void);
char *gpx_getClose(void);

#ifdef __cplusplus
}
#endif

#endif
