/**
*******************************************************************************
* @file nhm.h
* @author Keidan
* @par Project nhm
*
* @par Copyright
* Copyright 2015 Keidan, all right reserved
*
* This software is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY.
*
* Licence summary : 
*    You can modify and redistribute the sources code and binaries.
*    You can send me the bug-fix
*
* Term of the licence in in the file licence.txt.
*
*******************************************************************************
*/
#ifndef __NHM_H__
#define __NHM_H__

#include <linux/ioctl.h>

/* 
 * The major device number.
 */
#define NHM_MAJOR_NUMBER 100

/*
 * The device will appear at /dev/nhm using this value
 */
#define NHM_DEVICE_NAME "nhm"
/*
 * The device class -- this is a character device driver 
 */
#define NHM_CLASS_NAME  "nhm"


#define NHM_LEN_HW 6
#define NHM_LENGTH sizeof(struct nhm_s)

struct nhm_s {
    unsigned char  s_hw[NHM_LEN_HW];
    unsigned char  d_hw[NHM_LEN_HW];
    unsigned int   s_ip4;
    unsigned int   d_ip4;
    unsigned short s_port[2];
    unsigned short d_port[2];
    unsigned short eth_proto;
    unsigned short ip_proto;
};

#define nhm_to_ip(n0, n1, n2, n3) (((n0 & 0xFF) << 24) | ((n1 & 0xFF) << 16) | ((n2 & 0xFF) << 8) | (n3 & 0xFF))
#define nhm_from_ip(bytes, offset, value) ({	\
    bytes[offset + 3] = ((value >> 24) & 0xFF);	\
    bytes[offset + 2] = ((value >> 16) & 0xFF);	\
    bytes[offset + 1] = ((value >> 8) & 0xFF);	\
    bytes[offset] = (value & 0xFF);		\
  })

#define nhm_is_same(n1, n2) (memcmp((n1)->s_hw, (n2)->s_hw, NHM_LENGTH) == 0 \
			     && (n1)->s_ip4 == (n2)->s_ip4		\
			     && memcmp((n1)->s_port, (n2)->s_port,	\
				       2*sizeof(unsigned short)) == 0	\
			     && (n1)->eth_proto == (n2)->eth_proto	\
			     && (n1)->ip_proto == (n2)->ip_proto)

/* 
 * Adds a new entry
 */
#define NHM_IOCTL_ADD    _IOW(NHM_MAJOR_NUMBER, 0, struct nhm_s *)
/* 
 * Removes an entry
 */
#define NHM_IOCTL_DEL    _IOW(NHM_MAJOR_NUMBER, 1, struct nhm_s *)
/* 
 * Removes all entries.
 */
#define NHM_IOCTL_CLEAR  _IOW(NHM_MAJOR_NUMBER, 2, void *)
/* 
 * Clear the read index.
 */
#define NHM_IOCTL_ZERO   _IOW(NHM_MAJOR_NUMBER, 3, void *)
/*
 * Get the read length
 */
#define NHM_IOCTL_LENGTH _IOR(NHM_MAJOR_NUMBER, 4, int *)

#endif /* __NHM_H__ */
