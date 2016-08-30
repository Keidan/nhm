/**
*******************************************************************************
* @file nhm.h
* @author Keidan
* @par Project nhm
* @copyright Copyright 2015 Keidan, all right reserved.
* @par License:
* This software is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY.
*
* Licence summary :
*    You can modify and redistribute the sources code and binaries.
*    You can send me the bug-fix
*
* Term of the licence in in the file licence.txt.
*
*  _______          __                       __
*  \      \   _____/  |___  _  _____________|  | __
*  /   |   \_/ __ \   __\ \/ \/ /  _ \_  __ \  |/ /
* /    |    \  ___/|  |  \     (  <_> )  | \/    <
* \____|__  /\___  >__|   \/\_/ \____/|__|  |__|_ \
*         \/     \/                              \/
*   ___ ___                __
*  /   |   \  ____   ____ |  | __
* /    ~    \/  _ \ /  _ \|  |/ /
* \    Y    (  <_> |  <_> )    <
*  \___|_  / \____/ \____/|__|_ \
*        \/                    \/
*    _____             .___    .__
*   /     \   ____   __| _/_ __|  |   ____
*  /  \ /  \ /  _ \ / __ |  |  \  | _/ __ \
* /    Y    (  <_> ) /_/ |  |  /  |_\  ___/
* \____|__  /\____/\____ |____/|____/\___  >
*         \/            \/               \/
*
*******************************************************************************
*/
#ifndef __NHM_H__
#define __NHM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <linux/ioctl.h>
#ifndef _LINUX_MODULE_H
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#else
#include <linux/ioctl.h>
#include <linux/time.h>
#endif

/****************************************************
 * _________ ________    _______ ___________.___  ________
 * \_   ___ \\_____  \   \      \\_   _____/|   |/  _____/
 * /    \  \/ /   |   \  /   |   \|    __)  |   /   \  ___
 * \     \___/    |    \/    |    \     \   |   \    \_\  \
 *  \______  |_______  /\____|__  |___  /   |___|\______  /
 *         \/        \/         \/    \/                \/
 *****************************************************/

/**
 * @def NHM_MAJOR_NUMBER
 * @brief The major device number.
 */
#define NHM_MAJOR_NUMBER 100

/**
 * @def NHM_DEVICE_NAME
 * @brief The device will appear at /dev/nhm using this value.
 */
#define NHM_DEVICE_NAME "nhm"

/**
 * @def NHM_DEVICE_PATH
 * @brief The device path.
 */
#define NHM_DEVICE_PATH "/dev/"NHM_DEVICE_NAME

/**
 * @def NHM_CLASS_NAME
 * @brief The device class -- this is a character device driver.
 */
#define NHM_CLASS_NAME  "nhm"

/**
 * @def IFNAMSIZ
 * @brief The maximum length of the device name.
 */
#ifndef IFNAMSIZ
#define IFNAMSIZ        16
#endif

/**
 * @def IFALIASZ
 * @breif The maximum length of the device alias name.
 */
#ifndef IFALIASZ
#define IFALIASZ        256
#endif

/**
 * @def NHM_LEN_HW
 * @brief The length of the MAC address representation.
 */
#define NHM_LEN_HW 6

/**
 * @def NHM_LEN_IPv6
 * @brief The length of the IPv6 address representation.
 */
#define NHM_LEN_IPv6 16

/**
 * @def NHM_NULL_HW
 * @brief NULL or empty MAC address.
 */
#define NHM_NULL_HW {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

/**
 * @def NHM_NULL_IPv6
 * @brief NULL or empty IPv6 address.
 */
#define NHM_NULL_IPv6 {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

/**
 * @def NHM_LENGTH
 * @brief The length of the structure nhm_s.
 */
#define NHM_LENGTH sizeof(struct nhm_s)


/****************************************************
 * ________      _____________________    _________
 * \______ \    /  _  \__    ___/  _  \  /   _____/
 *  |    |  \  /  /_\  \|    | /  /_\  \ \_____  \
 *  |    `   \/    |    \    |/    |    \/	  \
 * /_______  /\____|__  /____|\____|__  /_______  /
 *         \/         \/              \/        \/
 *****************************************************/
/**
 * @enum nhm_nf_type_te
 * @brief Netfilter types.
 *
 * Defines the behaviour of the netfilter stack when a packet is processed.
 */
typedef enum {
  NHM_NF_TYPE_DROP = 0,                   /*!< Drop the packet; don't continue traversal. */
  NHM_NF_TYPE_ACCEPT,                     /*!< Continue traversal as normal. */
  NHM_NF_TYPE_STOLEN,                     /*!< I've taken over the packet; don't continue traversal. */
  NHM_NF_TYPE_QUEUE,                      /*!< Queue the packet (usually for userspace handling). */
  NHM_NF_TYPE_REPEAT,                     /*!< Call this hook again. */
  NHM_NF_TYPE_STOP                        /*!< NF_STOP is similar to NF_STOLEN , only difference is function callback is called on return in case of NF_STOP. */
} nhm_nf_type_te;

/**
 * @enum nhm_dir_te
 * @brief Rule direction.
 *
 * Defines the direction of the current rule.
 */
typedef enum {
  NHM_DIR_BOTH = 0,                       /*!< The rule is applied for the input and the output packets. */
  NHM_DIR_INPUT,                          /*!< The rule is applied for the input packets. */
  NHM_DIR_OUTPUT                          /*!< The rule is applied for the output packets. */
} nhm_dir_te;

/**
 * @struct nhm_s
 * @brief Rule representation.
 */
struct nhm_s {
    char           dev[IFNAMSIZ];         /*!< Network device name */
    nhm_nf_type_te nf_type;               /*!< Netfilter policy. */
    nhm_dir_te     dir;                   /*!< Packet direction */
    unsigned char  hw[NHM_LEN_HW];        /*!< Hardware MAC address. */
    unsigned int   ip4;                   /*!< IPv4 address. */
    unsigned char  ip6[NHM_LEN_IPv6];     /*!< IPv6 address. */
    unsigned short port[2];               /*!< Port value or range. */
    unsigned short eth_proto;             /*!< Ethernet protocol ID */
    unsigned short ip_proto;              /*!< IP protocol ID */
    struct {
        unsigned long   counter;          /*!< Simple counter to obtain the number of times the rule is applied.*/
        struct timespec last;             /*!< Last time where the rule is applied. */
    }              applied;               /*!< Informations when the rule is applied. */
};

/****************************************************
 *  ____ ______________.___.____       _________
 * |    |   \__    ___/|   |    |     /   _____/
 * |    |   / |    |   |   |    |     \_____  \
 * |    |  /  |    |   |   |    |___  /	       \
 * |______/   |____|   |___|_______ \/_______  /
 *                                 \/        \/
 *****************************************************/
/**
 * @def nhm_to_ipv4(n0, n1, n2, n3)
 * @brief Transform 4 unsigned chars to an unsigned int.
 * @param[in] n0 bits 0-8
 * @param[in] n1 bits 8-16
 * @param[in] n2 bits 16-24
 * @param[in] n3 bits 24-32
 * @return An unsigned int
 */
#define nhm_to_ipv4(n0, n1, n2, n3) (((n3 & 0xFF) << 24) | ((n2 & 0xFF) << 16) | ((n1 & 0xFF) << 8) | (n0 & 0xFF))

/**
 * @def nhm_from_ipv4(bytes, offset, value)
 * @brief Transform an unsigned int to an unsigned char array (4).
 * @param[out] bytes The output array.
 * @param[in] offset The possible bytes offset.
 * @param[in] value The value the transform.
 */
#define nhm_from_ipv4(bytes, offset, value) do {	\
    bytes[offset + 3] = ((value >> 24) & 0xFF);		\
    bytes[offset + 2] = ((value >> 16) & 0xFF);		\
    bytes[offset + 1] = ((value >> 8) & 0xFF);		\
    bytes[offset] = (value & 0xFF);			\
  } while(0)

/**
 * @def nhm_is_same(n1, n2)
 * @brief Test if the first rule equals the second.
 * @param[in] n1 The first rule to test with the second rule.
 * @param[in] n2 The second rule to test with the first rule.
 * @return 1 if the rules are equals, 0 else.
 */
#define nhm_is_same(n1, n2) (memcmp((n1)->dev, (n2)->dev, IFNAMSIZ) == 0 \
                 && memcmp((n1)->hw, (n2)->hw, NHM_LENGTH) == 0 \
                 && memcmp((n1)->ip6, (n2)->ip6, NHM_LEN_IPv6) == 0 \
                 && (n1)->ip4 == (n2)->ip4			\
                 && memcmp((n1)->port, (n2)->port,		\
                       2*sizeof(unsigned short)) == 0	\
                 && (n1)->eth_proto == (n2)->eth_proto	\
                 && (n1)->ip_proto == (n2)->ip_proto)


/****************************************************
 * .___________  ____________________.____
 * |   \_____  \ \_   ___ \__    ___/|    |
 * |   |/   |   \/    \  \/ |    |   |    |
 * |   /    |    \     \____|    |   |    |___
 * |___\_______  /\______  /|____|   |_______	\
 *             \/        \/                  \/
 *****************************************************/

/**
 * @def NHM_IOCTL_ADD
 * @brief Adding a new entry pointed by the IOCTL argument.
 */
#define NHM_IOCTL_ADD    _IOW(NHM_MAJOR_NUMBER, 0, struct nhm_s *)

/**
 * @def NHM_IOCTL_DEL
 * @brief Removing an existing entry pointed by the IOCTL argument.
 */
#define NHM_IOCTL_DEL    _IOW(NHM_MAJOR_NUMBER, 1, struct nhm_s *)

/**
 * @def NHM_IOCTL_CLEAR
 * @brief Removing all entries.
 */
#define NHM_IOCTL_CLEAR  _IOR(NHM_MAJOR_NUMBER, 2, void *)

/**
 * @def NHM_IOCTL_ZERO
 * @brief Changes the read index and places it to the head of the list.
 */
#define NHM_IOCTL_ZERO   _IOR(NHM_MAJOR_NUMBER, 3, void *)

/**
 * @def NHM_IOCTL_LENGTH
 * @brief Returns the number of items that the read function can return.
 */
#define NHM_IOCTL_LENGTH _IOR(NHM_MAJOR_NUMBER, 4, int *)

/**
 * @def NHM_IOCTL_NF_TYPE
 * @brief Changes the globale netfilter type to be apply by default.
 */
#define NHM_IOCTL_NF_TYPE _IOW(NHM_MAJOR_NUMBER, 5, nhm_nf_type_te *)






/*****************************************************
 *   ___ ___        .__
 *  /   |   \  ____ |  | ______   ___________  ______
 * /    ~    \/ __ \|  | \____ \_/ __ \_  __ \/  ___/
 * \    Y    |  ___/|  |_|  |_> >  ___/|  | \/\___ \
 *  \___|_  / \___  >____/   __/ \___  >__|  /____  >
 *        \/      \/     |__|        \/           \/
 *****************************************************/

/**
 * @def nhm_open()
 * @brief After this call an connection between the module and the user application will be opened.
 * @code
 * int fd;
 * ...
 * fd = nhm_open();
 * ...
 * @endcode
 * @return The return code of the open function (man open).
 */
#define nhm_open()                 open(NHM_DEVICE_PATH, O_RDWR)

/**
 * @def nhm_close(fd)
 * @brief After this call the connection between the module and the user application will be closed.
 * @code
 * int fd;
 * ...
 * nhm_close(fd);
 * ...
 * @endcode
 * @param[in] fd The fd between the user application and the module.
 * @return The return code of the close function (man close).
 */
#define nhm_close(fd)              close(fd)

/**
 * @def nhm_get_rule(fd, r)
 * @brief Read a rule from the module
 * @code
 * struct nhm_s r;
 * int fd, length, ret, i;
 * ...
 * ret = nhm_rules_size(fd, &length);
 * ...
 * for(i = 0; i < length; i++) {
 *   ret = nhm_get_rule(fd, &r);
 *   // do stuff
 *   ...
 * }
 * ...
 * @endcode
 * @param[in] fd The fd between the user application and the module.
 * @param[out] r The rule.
 * @return The return code of the read function (man read).
 */
#define nhm_get_rule(fd, r)        read(fd, r, NHM_LENGTH)

/**
 * @def nhm_init_rule(r)
 * @brief Initializes all the fields of the rule to 0
 * @code
 * struct nhm_s r;
 * ...
 * nhm_init_rule(&r);
 * ...
 * @endcode
 * @param[out] r The rule to read.
 * @return The return code of the memset function (man memset).
 */
#define nhm_init_rule(r)           memset(r, 0, NHM_LENGTH)

/**
 * @def nhm_add_rule(fd, r)
 * @brief After this call all packets which would correspond to the rule configuration will be treated.
 * @code
 * struct nhm_s r;
 * int fd, ret;
 * ...
 * nhm_init_rule(&r);
 * r.ip4 = nhm_to_ipv4(192, 168, 0, 1);
 * r.dir = NHM_DIR_OUTPUT;
 * r.nf_type = NHM_NF_TYPE_DROP;
 * int ret = nhm_add_rule(fd, &r);
 * ...
 * @endcode
 * @param[in] fd The fd between the user application and the module.
 * @param[in] r The rule to add.
 * @return The return code of the ioctl function (man ioctl).
 */
#define nhm_add_rule(fd, r)        ioctl(fd, NHM_IOCTL_ADD, r)

/**
 * @def nhm_del_rule(fd, r)
 * @brief After this call the rule will be removed.
 * @code
 * struct nhm_s r;
 * int fd, ret;
 * ...
 * nhm_init_rule(&r);
 * r.ip4 = nhm_to_ipv4(192, 168, 0, 1);
 * r.dir = NHM_DIR_OUTPUT;
 * r.nf_type = NHM_NF_TYPE_DROP;
 * int ret = nhm_del_rule(fd, &r);
 * ...
 * @endcode
 * @param[in] fd The fd between the user application and the module.
 * @param[in] r The rule to remove.
 * @return The return code of the ioctl function (man ioctl).
 */
#define nhm_del_rule(fd, r)        ioctl(fd, NHM_IOCTL_DEL, r)

/**
 * @def nhm_clear_rules(fd)
 * @brief After this call all the rules will be removed.
 * @code
 * int fd, ret;
 * ...
 * int ret = nhm_clear_rules(fd);
 * ...
 * @endcode
 * @param[in] fd The fd between the user application and the module.
 * @return The return code of the ioctl function (man ioctl).
 */
#define nhm_clear_rules(fd)        ioctl(fd, NHM_IOCTL_CLEAR, NULL)

/**
 * @def nhm_read_rewind(fd)
 * @brief After this call the read index will be reseted to the head of the list.
 * @code
 * int fd, ret;
 * ...
 * int ret = nhm_read_rewind(fd);
 * ...
 * @endcode
 * @param[in] fd The fd between the user application and the module.
 * @return The return code of the ioctl function (man ioctl).
 */
#define nhm_read_rewind(fd)        ioctl(fd, NHM_IOCTL_ZERO, NULL)

/**
 * @def nhm_rules_size(fd, length)
 * @brief After this call the length field contain the number of inserted elements.
 * @code
 * int fd, ret, length;
 * ...
 * int ret = nhm_rules_size(fd, &length);
 * ...
 * @endcode
 * @param[in] fd The fd between the user application and the module.
 * @param[out] length The numbere of elements.
 * @return The return code of the ioctl function (man ioctl)
 */
#define nhm_rules_size(fd, length) ioctl(fd, NHM_IOCTL_LENGTH, length)

/**
 * @def nhm_set_netfilter_type(fd, type)
 * @brief After this call all the packets will be managed by default with the value of the type parameter.
 * @code
 * nhm_nf_type_te type = NHM_NF_TYPE_DROP; // drop all packets.
 * nhm_set_netfilter_type(fd, &type);
 * @endcode
 * @param[in] fd The fd between the user application and the module.
 * @param[in] type The behaviour type.
 * @return The return code of the ioctl function (man ioctl).
 */
#define nhm_set_netfilter_type(fd, type) ioctl(fd, NHM_IOCTL_NF_TYPE, type)

#ifdef __cplusplus
}
#endif

#endif /* __NHM_H__ */
