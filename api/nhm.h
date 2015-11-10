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

typedef enum {
  NHM_NF_TYPE_DROP = 0, /* Drop the packet; don't continue traversal. */
  NHM_NF_TYPE_ACCEPT,   /* Continue traversal as normal. */
  NHM_NF_TYPE_STOLEN,   /* I've taken over the packet; don't continue traversal. */
  NHM_NF_TYPE_QUEUE,    /* Queue the packet (usually for userspace handling). */
  NHM_NF_TYPE_REPEAT,   /* Call this hook again. */
  NHM_NF_TYPE_STOP      /* NF_STOP is similar to NF_STOLEN , only difference is function callback is called on return in case of NF_STOP. */
} nhm_nf_type_te;

typedef enum {
  NHM_DIR_BOTH = 0,
  NHM_DIR_INPUT,
  NHM_DIR_OUTPUT
} nhm_dir_te;

struct nhm_s {
    nhm_nf_type_te nf_type;
    nhm_dir_te     dir;
    unsigned char  hw[NHM_LEN_HW];
    unsigned int   ip4;
    unsigned short port[2];
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

#define nhm_is_same(n1, n2) (memcmp((n1)->hw, (n2)->hw, NHM_LENGTH) == 0 \
			     && (n1)->ip4 == (n2)->ip4			\
			     && memcmp((n1)->port, (n2)->port,		\
				       2*sizeof(unsigned short)) == 0	\
			     && (n1)->eth_proto == (n2)->eth_proto	\
			     && (n1)->ip_proto == (n2)->ip_proto)

/* 
 * Adding a new entry pointed by the IOCTL argument.
 * @code
 * struct nhm_s n;
 * memset(&n, 0, NHM_LENGTH);
 * n.ip4 = nhm_to_ip(192, 168, 0, 1);
 * n.dir = NHM_DIR_OUTPUT;
 * n.nf_type = NHM_NF_TYPE_DROP;
 * // After this call all packets from the ip 192.168.0.1 or to the ip 192.168.0.1 will be ignored.
 * ioctl(fd, NHM_IOCTL_ADD, &n);
 * @endcode
 */
#define NHM_IOCTL_ADD    _IOW(NHM_MAJOR_NUMBER, 0, struct nhm_s *)
/* 
 * Updates an existing entry pointed by the IOCTL argument.
 * @code
 * struct nhm_s n;
 * memset(&n, 0, NHM_LENGTH);
 * n.ip4 = nhm_to_ip(192, 168, 0, 1);
 * n.dir = NHM_DIR_INPUT;
 * n.nf_type = NHM_NF_TYPE_DROP;
 * // After this call all packets from the ip 192.168.0.1 or 
 * // to the ip 192.168.0.1 will be ignored if they match with the input direction..
 * ioctl(fd, NHM_IOCTL_SET, &n);
 * @endcode
 */
#define NHM_IOCTL_SET    _IOW(NHM_MAJOR_NUMBER, 1, struct nhm_s *)
/* 
 * Removing an existing entry pointed by the IOCTL argument.
 * @code
 * struct nhm_s n;
 * memset(&n, 0, NHM_LENGTH);
 * n.ip4 = nhm_to_ip(192, 168, 0, 1);
 * // After this call all packets from the ip 192.168.0.1 or to the ip 192.168.0.1 will be accepted.
 * ioctl(fd, NHM_IOCTL_DEL, &n);
 * @endcode
 */
#define NHM_IOCTL_DEL    _IOW(NHM_MAJOR_NUMBER, 2, struct nhm_s *)
/* 
 * Removing all entries.
 * @code
 * // After this call all entries will be removed.
 * ioctl(fd, NHM_IOCTL_CLEAR, NULL);
 * @endcode
 */
#define NHM_IOCTL_CLEAR  _IOW(NHM_MAJOR_NUMBER, 3, void *)
/* 
 * Changes the read index and places it to the head of the list.
 * @code
 * // After this call the read index will be reseted to the head of the list.
 * ioctl(fd, NHM_IOCTL_ZERO, NULL);
 * @endcode
 */
#define NHM_IOCTL_ZERO   _IOW(NHM_MAJOR_NUMBER, 4, void *)
/*
 * Returns the number of items that the read function can return.
 * @code
 * int length;
 * // After this call the length field contain the number of inserted elements.
 * ioctl(fd, NHM_IOCTL_LENGTH, &length);
 * @endcode
 */
#define NHM_IOCTL_LENGTH _IOR(NHM_MAJOR_NUMBER, 5, int *)
/*
 * Changes the globale netfilter type to be apply by default.
 * @code
 * nhm_nf_type_te type = NHM_NF_TYPE_DROP;
 * // After this call all the packets will be dropped by default.
 * ioctl(fd, NHM_IOCTL_NF_TYPE, NULL);
 * @endcode
 */
#define NHM_IOCTL_NF_TYPE _IOW(NHM_MAJOR_NUMBER, 6, nhm_nf_type_te *)

#endif /* __NHM_H__ */
