#include "utf8_gb2312_switch.h"
#include "ff.h"

extern const WCHAR uni2oem[43586];

/*---------------------------------*/

/* Converted code, 0 means conversion error */
/* Character code to be converted */
/* 0: Unicode to OEM code, 1: OEM code to Unicode */
static uint16_t usg_convert (uint16_t chr, int dir)
{
	const uint16_t *p;
	uint16_t c;
	int i, n, li, hi;


	if (chr < 0x80) {	/* ASCII */
		c = chr;
	} else {
		if (dir) {		/* OEM code to unicode */
			p = uni2oem;
			hi = sizeof (uni2oem) / 4 - 1;
		} else {		/* Unicode to OEM code */
			p = uni2oem;
			hi = sizeof (uni2oem) / 4 - 1;
		}
		li = 0;
		for (n = 16; n; n--) {
			i = li + (hi - li) / 2;
			if (chr == p[i * 2]) break;
			if (chr > p[i * 2])
				li = i;
			else
				hi = i;
		}
		c = n ? p[i * 2 + 1] : 0;
	}

	return c;
}
/*---------------------------------*/

static int usg_get_utf8_size(const unsigned char pInput)
{
    unsigned char c = pInput;
    // 0xxxxxxx return 0
    // 10xxxxxx no exsit
    // 110xxxxx return 2
    // 1110xxxx return 3
    // 11110xxx return 4
    // 111110xx return 5
    // 1111110x return 6
    if(c< 0x80) return 0;
    if(c>=0x80 && c<0xC0) return -1;
    if(c>=0xC0 && c<0xE0) return 2;
    if(c>=0xE0 && c<0xF0) return 3;
    if(c>=0xF0 && c<0xF8) return 4;
    if(c>=0xF8 && c<0xFC) return 5;
    if(c>=0xFC) return 6;
    return 0;
}


int usg_unicode_to_utf8_one(unsigned long unic, unsigned char *pOutput, int outSize)  
{  
  assert(pOutput != NULL);  
  assert(outSize >= 6);  

  if ( unic <= 0x0000007F )  
  {  
    // * U-00000000 - U-0000007F:  0xxxxxxx  
    *pOutput     = (unic & 0x7F);  
    return 1;  
  }  
  else if ( unic >= 0x00000080 && unic <= 0x000007FF )  
  {  
    // * U-00000080 - U-000007FF:  110xxxxx 10xxxxxx  
    *(pOutput+1) = (unic & 0x3F) | 0x80;  
    *pOutput     = ((unic >> 6) & 0x1F) | 0xC0;  
    return 2;  
  }  
  else if ( unic >= 0x00000800 && unic <= 0x0000FFFF )  
  {  
    // * U-00000800 - U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx  
    *(pOutput+2) = (unic & 0x3F) | 0x80;  
    *(pOutput+1) = ((unic >>  6) & 0x3F) | 0x80;  
    *pOutput     = ((unic >> 12) & 0x0F) | 0xE0;  
    return 3;  
  }  
  else if ( unic >= 0x00010000 && unic <= 0x001FFFFF )  
  {  
    // * U-00010000 - U-001FFFFF:  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx  
    *(pOutput+3) = (unic & 0x3F) | 0x80;  
    *(pOutput+2) = ((unic >>  6) & 0x3F) | 0x80;  
    *(pOutput+1) = ((unic >> 12) & 0x3F) | 0x80;  
    *pOutput     = ((unic >> 18) & 0x07) | 0xF0;  
    return 4;  
  }  
  else if ( unic >= 0x00200000 && unic <= 0x03FFFFFF )  
  {  
    // * U-00200000 - U-03FFFFFF:  111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx  
    *(pOutput+4) = (unic & 0x3F) | 0x80;  
    *(pOutput+3) = ((unic >>  6) & 0x3F) | 0x80;  
    *(pOutput+2) = ((unic >> 12) & 0x3F) | 0x80;  
    *(pOutput+1) = ((unic >> 18) & 0x3F) | 0x80;  
    *pOutput     = ((unic >> 24) & 0x03) | 0xF8;  
    return 5;  
  }  
  else if ( unic >= 0x04000000 && unic <= 0x7FFFFFFF )  
  {  
    // * U-04000000 - U-7FFFFFFF:  1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx  
    *(pOutput+5) = (unic & 0x3F) | 0x80;  
    *(pOutput+4) = ((unic >>  6) & 0x3F) | 0x80;  
    *(pOutput+3) = ((unic >> 12) & 0x3F) | 0x80;  
    *(pOutput+2) = ((unic >> 18) & 0x3F) | 0x80;  
    *(pOutput+1) = ((unic >> 24) & 0x3F) | 0x80;  
    *pOutput     = ((unic >> 30) & 0x01) | 0xFC;  
    return 6;  
  }  

  return 0;  
}  

int usg_utf8_to_unicode_one(const unsigned char* pInput, unsigned long *Unic)  
{  
  assert(pInput != NULL && Unic != NULL);  

  char b1, b2, b3, b4, b5, b6;  

  *Unic = 0x0; 
  int utfbytes = usg_get_utf8_size(*pInput);  
  unsigned char *pOutput = (unsigned char *) Unic;  

  switch ( utfbytes )  
  {  
    case 0:  
      *pOutput     = *pInput;  
      utfbytes    += 1;  
      break;  
      case 2:  
      b1 = *pInput;  
      b2 = *(pInput + 1);  
      if ( (b2 & 0xE0) != 0x80 )  
        return 0;  
      *pOutput     = (b1 << 6) + (b2 & 0x3F);  
      *(pOutput+1) = (b1 >> 2) & 0x07;  
      break;  
    case 3:  
      b1 = *pInput;  
      b2 = *(pInput + 1);  
      b3 = *(pInput + 2);  
      if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80) )  
        return 0;  
      *pOutput     = (b2 << 6) + (b3 & 0x3F);  
      *(pOutput+1) = (b1 << 4) + ((b2 >> 2) & 0x0F);  
      break;  
    case 4:  
      b1 = *pInput;  
      b2 = *(pInput + 1);  
      b3 = *(pInput + 2);  
      b4 = *(pInput + 3);  
      if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)  
      || ((b4 & 0xC0) != 0x80) )  
        return 0;  
      *pOutput     = (b3 << 6) + (b4 & 0x3F);  
      *(pOutput+1) = (b2 << 4) + ((b3 >> 2) & 0x0F);  
      *(pOutput+2) = ((b1 << 2) & 0x1C)  + ((b2 >> 4) & 0x03);  
      break;  
    case 5:  
      b1 = *pInput;  
      b2 = *(pInput + 1);  
      b3 = *(pInput + 2);  
      b4 = *(pInput + 3);  
      b5 = *(pInput + 4);  
      if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)  
      || ((b4 & 0xC0) != 0x80) || ((b5 & 0xC0) != 0x80) )  
        return 0;  
      *pOutput     = (b4 << 6) + (b5 & 0x3F);  
      *(pOutput+1) = (b3 << 4) + ((b4 >> 2) & 0x0F);  
      *(pOutput+2) = (b2 << 2) + ((b3 >> 4) & 0x03);  
      *(pOutput+3) = (b1 << 6);  
      break;  
    case 6:  
      b1 = *pInput;  
      b2 = *(pInput + 1);  
      b3 = *(pInput + 2);  
      b4 = *(pInput + 3);  
      b5 = *(pInput + 4);  
      b6 = *(pInput + 5);  
      if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)  
      || ((b4 & 0xC0) != 0x80) || ((b5 & 0xC0) != 0x80)  
      || ((b6 & 0xC0) != 0x80) )  
        return 0;  
      *pOutput     = (b5 << 6) + (b6 & 0x3F);  
      *(pOutput+1) = (b5 << 4) + ((b6 >> 2) & 0x0F);  
      *(pOutput+2) = (b3 << 2) + ((b4 >> 4) & 0x03);  
      *(pOutput+3) = ((b1 << 6) & 0x40) + (b2 & 0x3F);  
      break;  
    default:  
      return 0;  
  }  

  return utfbytes;  
}

/**
  * @brief  Utf8 to gb2312.
  * @param  [in ] utf8Data   : utf-8 data
  *         [in ] len        : utf-8 data len
  *         [out] gb2312Data : converted gb2312 Data
  *         [in ] size       : gb2312Data buf size
  *  
  * @retval > 0 : gb2312Data len
  * @retval = 0 : erron
  */
size_t utf8_to_gb2312(const uint8_t *utf8Data, size_t len, uint8_t *gb2312Data, size_t size)
{
    char Unic[8] = {0};
    uint16_t ptr1, ptr2;
    int ret;
    size_t offset = 0;

    while(len > 0)
    {
        memset(Unic, 0, sizeof(Unic));
        ret = usg_utf8_to_unicode_one((uint8_t *)utf8Data, (unsigned long *)Unic);
        if(ret > 0) {
            if(ret == 1)  {
                gb2312Data[offset++] = *utf8Data;
            } else {
                ptr1 = (Unic[1] << 8) | Unic[0];
                ptr2 = usg_convert(ptr1, 0);
                if(offset >= size) break;
                gb2312Data[offset++] = (ptr2 >> 8) & 0xff ;
                gb2312Data[offset++] = (ptr2 >> 0) & 0xff ;
            }
            if( len < ret ) break;
            len -= ret;
            utf8Data += ret;
        } else  {
            break;
        }
    }
    return offset;
}

/**
  * @brief  gb2312 to Utf8.
  * @param  [in ] gb2312Data : gb2312 data
  *         [in ] len        : gb2312 data len
  *         [out] utf8Data   : converted utf-8 Data
  *         [in ] size       : utf-8 buf size
  *  
  * @retval > 0 : utf8Data len
  * @retval = 0 : erron
  */
size_t gb2312_to_utf8(const uint8_t *gb2312Data, size_t len, uint8_t *utf8Data, size_t size)
{
    uint8_t output[8] = {0};
    uint16_t ptr1;
    int ret;
    size_t offset = 0;
    int gsize = 0;

    while(len > 0)
    {
        if(usg_get_utf8_size(*gb2312Data) == 0) {
            gsize = 1;
            ptr1 = *gb2312Data;
        } else {
            gsize = 2;
            ptr1 = gb2312Data[0] << 8 | gb2312Data[1];
        }
        ptr1 = usg_convert(ptr1, 1);
        memset(output, 0, sizeof(output));
        ret = usg_unicode_to_utf8_one(ptr1, output, sizeof(output));
        if(ret > 0) {
            gb2312Data += gsize;
            for(int i=0;i<ret;i++)
            {
                utf8Data[offset+i] = output[i];
            }
            offset += ret;
            if( len < gsize ) break;
            len -= gsize;
        } else  {
            break;
        }
    }
    return offset;
}
