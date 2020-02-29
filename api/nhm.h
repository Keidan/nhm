/**
 * @file nhm.h
 * @author Keidan
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3
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
 */
#ifndef __NHM_H__
#define __NHM_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _LINUX_MODULE_H
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
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
 * @brief The major device number.
 */
#define NHM_MAJOR_NUMBER 100

/**
 * @brief Using this value the device will appear in /dev/nhm.
 */
#define NHM_DEVICE_NAME "nhm"

/**
 * @brief The device path.
 */
#define NHM_DEVICE_PATH "/dev/" NHM_DEVICE_NAME

/**
 * @brief The device class -- this is a character device driver.
 */
#define NHM_CLASS_NAME  "nhm"

/**
 * @brief The maximum length of the device name.
 */
#ifndef IFNAMSIZ
  #define IFNAMSIZ      16
#endif

/**
 * @breif The maximum length of the device alias name.
 */
#ifndef IFALIASZ
  #define IFALIASZ      256
#endif

/**
 * @brief The length of the MAC address representation.
 */
#define NHM_LEN_HW      6

/**
 * @brief The length of the IPv6 address representation.
 */
#define NHM_LEN_IPv6    16

/**
 * @brief NULL or empty MAC address.
 */
#define NHM_NULL_HW     { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

/**
 * @brief NULL or empty IPv6 address.
 */
#define NHM_NULL_IPv6   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                         0x00, 0x00, 0x00, 0x00 }

/**
 * @brief The length of the nhm_s structure.
 */
#define NHM_LENGTH sizeof(struct nhm_s)


/****************************************************
 * ________      _____________________ 
 * \______ \    /  _  \__    ___/  _  \
 *  |    |  \  /  /_\  \|    | /  /_\  \
 *  |    `   \/    |    \    |/    |    \
 * /_______  /\____|__  /____|\____|__  /
 *         \/         \/              \/
 *****************************************************/
/**
 * @brief Netfilter types.
 *
 *Defines the behavior of the netfilter stack when a packet is processed.
 */
typedef enum 
{
  NHM_NF_TYPE_DROP = 0,                   /*!< Drop the packet; don't continue traversal. */
  NHM_NF_TYPE_ACCEPT,                     /*!< Continue traversal as normal. */
  NHM_NF_TYPE_STOLEN,                     /*!< I've taken over the packet; don't continue traversal. */
  NHM_NF_TYPE_QUEUE,                      /*!< Queue the packet (usually for userspace handling). */
  NHM_NF_TYPE_REPEAT,                     /*!< Call this hook again. */
  NHM_NF_TYPE_STOP                        /*!< NF_STOP is similar to NF_STOLEN , only difference is function callback is called on return in case of NF_STOP. */
} nhm_nf_type_te;

/**
 * @brief Rule direction.
 *
 * Defines the direction of the current rule.
 */
typedef enum 
{
  NHM_DIR_BOTH = 0,                       /*!< The rule is applied for the input and the output packets. */
  NHM_DIR_INPUT,                          /*!< The rule is applied for the input packets. */
  NHM_DIR_OUTPUT                          /*!< The rule is applied for the output packets. */
} nhm_dir_te;

/**
 * @brief Rule representation.
 */
struct nhm_s 
{
  char           dev[IFNAMSIZ];           /*!< Network device name */
  nhm_nf_type_te nf_type;                 /*!< Netfilter policy. */
  nhm_dir_te     dir;                     /*!< Packet direction */
  unsigned char  hw[NHM_LEN_HW];          /*!< Hardware MAC address. */
  unsigned int   ip4;                     /*!< IPv4 address. */
  unsigned char  ip6[NHM_LEN_IPv6];       /*!< IPv6 address. */
  unsigned short port[2];                 /*!< Port value or range. */
  unsigned short eth_proto;               /*!< Ethernet protocol ID */
  unsigned short ip_proto;                /*!< IP protocol ID */
  struct 
  {
    unsigned long   counter;              /*!< Simple counter to obtain the number of times the rule is applied.*/
    struct timespec last;                 /*!< Last time where the rule is applied. */
  }              applied;                 /*!< Informations when the rule is applied. */
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
 * @brief Transforms 4 unsigned characters into an unsigned int.
 * @param[in] n0 bits 0-8
 * @param[in] n1 bits 8-16
 * @param[in] n2 bits 16-24
 * @param[in] n3 bits 24-32
 * @return An unsigned int
 */
#define nhm_to_ipv4(n0, n1, n2, n3) (((n3 & 0xFF) << 24) | ((n2 & 0xFF) << 16) | ((n1 & 0xFF) << 8) | (n0 & 0xFF))

/**
 * @brief Test if the IPv4 address (unsigned int representation) corresponds to an IPv4 address (type any)
 * @param[in] ip4 IPv4 to be tested.
 * @return 1 if the address is considered to correspond to "any", 0 otherwise
 */
#define nhm_is_ipv4_any(ip4) (!!(ip4 == 0))

/**
 * @brief Transforms an unsigned int into an array of unsigned characters (4).
 * @param[out] bytes The output array.
 * @param[in] offset The possible bytes offset.
 * @param[in] value The value of transformation.
 */
#define nhm_from_ipv4(bytes, offset, value) do {	\
    bytes[offset + 3] = ((value >> 24) & 0xFF);		\
    bytes[offset + 2] = ((value >> 16) & 0xFF);		\
    bytes[offset + 1] = ((value >> 8) & 0xFF);		\
    bytes[offset] = (value & 0xFF);			          \
  } while(0)

/**
 * @brief Test if the MAC address  corresponds to an MAC address (type any)
 * @param[in] mac MAC to be tested (unsigned char * 6).
 * @return 1 if the address is considered to correspond to "any", 0 otherwise
 */
#define nhm_is_mac_any(mac) (!!(mac[0] == 0 && mac[1] == 0 && \
                                mac[2] == 0 && mac[3] == 0 && \
                                mac[4] == 0 && mac[5] == 0))

/**
 * @brief Check if the first rule is equal to the second.
 * @param[in] n1 The first rule to test with the second rule.
 * @param[in] n2 The second rule to test with the first rule.
 * @return 1 if the rules are equal, 0 otherwise.
 */
#define nhm_is_same(n1, n2) (memcmp((n1)->dev, (n2)->dev, IFNAMSIZ) == 0 \
                 && memcmp((n1)->hw, (n2)->hw, NHM_LEN_HW) == 0 \
                 && memcmp((n1)->ip6, (n2)->ip6, NHM_LEN_IPv6) == 0 \
                 && (n1)->ip4 == (n2)->ip4			\
                 && memcmp((n1)->port, (n2)->port,		\
                       sizeof((n1)->port)) == 0	\
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
 * @brief Added a new entry pointed by the IOCTL argument.
 */
#define NHM_IOCTL_ADD    _IOW(NHM_MAJOR_NUMBER, 0, struct nhm_s *)

/**
 * @brief Deletes an existing entry pointed to by the IOCTL argument.
 */
#define NHM_IOCTL_DEL    _IOW(NHM_MAJOR_NUMBER, 1, struct nhm_s *)

/**
 * @brief Deletes all entries.
 */
#define NHM_IOCTL_CLEAR  _IOR(NHM_MAJOR_NUMBER, 2, void *)

/**
 * @brief Moves the index back to the top of the list.
 */
#define NHM_IOCTL_ZERO   _IOR(NHM_MAJOR_NUMBER, 3, void *)

/**
 * @brief Returns the number of items that the playback function can return.
 */
#define NHM_IOCTL_LENGTH _IOR(NHM_MAJOR_NUMBER, 4, int *)

/**
 * @brief Changes the global netfilter type to be applied by default.
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
 * @brief After this call, a connection between the module and the user application will be opened.
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
 * @brief After this call, the connection between the module and the user application will be closed.
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
 * @brief Reading a rule stored in the module
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
 * @param[out] r The rule to read.
 * @return The return code of the read function (man read).
 */
#define nhm_get_rule(fd, r)        read(fd, r, NHM_LENGTH)

/**
 * @brief Reset all fields of the rule to 0
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
 * @brief After this call, all packets that match the rule configuration will be processed.
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
 * @param[in] r The rule to be added.
 * @return The return code of the ioctl function (man ioctl).
 */
#define nhm_add_rule(fd, r)        ioctl(fd, NHM_IOCTL_ADD, r)

/**
 * @brief After this call, the rule will be deleted.
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
 * @param[in] r The rule to be deleted.
 * @return The return code of the ioctl function (man ioctl).
 */
#define nhm_del_rule(fd, r)        ioctl(fd, NHM_IOCTL_DEL, r)

/**
 * @brief After this call, all rules will be deleted.
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
 * @brief After this call, the index being read will be moved back to the top of the list.
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
 * @brief After this call, the length field will contain the number of inserted elements.
 * @code
 * int fd, ret, length;
 * ...
 * int ret = nhm_rules_size(fd, &length);
 * ...
 * @endcode
 * @param[in] fd The fd between the user application and the module.
 * @param[out] length The number of elements.
 * @return The return code of the ioctl function (man ioctl)
 */
#define nhm_rules_size(fd, length) ioctl(fd, NHM_IOCTL_LENGTH, length)

/**
 * @brief After this call, all packets will be managed by default with the value of the type parameter.
 * @code
 * nhm_nf_type_te type = NHM_NF_TYPE_DROP; // drop all packets.
 * nhm_set_netfilter_type(fd, &type);
 * @endcode
 * @param[in] fd The fd between the user application and the module.
 * @param[in] type The type of behavior.
 * @return The return code of the ioctl function (man ioctl).
 */
#define nhm_set_netfilter_type(fd, type) ioctl(fd, NHM_IOCTL_NF_TYPE, type)

#ifdef __cplusplus
}
#endif

#endif /* __NHM_H__ */
