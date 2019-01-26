/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/

#include "gfx.h"
#include "gfile_fatfs_wrapper.h"

/* Initialize a Drive                                                    */
DSTATUS disk_initialize (
    BYTE drv                /* Physical drive nmuber (0..) */
)
{
  DSTATUS stat;


  return STA_NODISK;
}



/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */

DSTATUS disk_status (
    BYTE drv        /* Physical drive nmuber (0..) */
)
{
  DSTATUS stat;


  return STA_NODISK;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */

DRESULT disk_read (
    BYTE drv,        /* Physical drive nmuber (0..) */
    BYTE *buff,        /* Data buffer to store read data */
    DWORD sector,    /* Sector address (LBA) */
    UINT count        /* Number of sectors to read (1..255) */
)
{

  return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */

#if _READONLY == 0
DRESULT disk_write (
    BYTE drv,            /* Physical drive nmuber (0..) */
    const BYTE *buff,    /* Data to be written */
    DWORD sector,        /* Sector address (LBA) */
    UINT count            /* Number of sectors to write (1..255) */
)
{

  return RES_PARERR;
}
#endif /* _READONLY */



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */

DRESULT disk_ioctl (
    BYTE drv,        /* Physical drive nmuber (0..) */
    BYTE ctrl,        /* Control code */
    void *buff        /* Buffer to send/receive control data */
)
{

  return RES_PARERR;
}


	DWORD get_fattime(void) {
	    return ((uint32_t)0 | (1 << 16)) | (1 << 21); /* wrong but valid time */
	}

