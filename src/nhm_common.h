#ifndef __NHM_COMMON_H__
  #define __NHM_COMMON_H__


  #include <linux/kobject.h>      /* Needed by kobject */
  #include <linux/sysfs.h>        /* Needed by sysfs_xxxx */
  #include <linux/string.h>       /* Needed by strings operations */

  #define LEN_HW 17
  #define LEN_IPv4 15
  #define LEN_PORTS 2


  struct nhm_net_entry_s {
      unsigned char sh[LEN_HW];
      unsigned char dh[LEN_HW];
      unsigned char si[LEN_IPv4];
      unsigned char di[LEN_IPv4];
      unsigned short sp[LEN_PORTS];
      unsigned short dp[LEN_PORTS];
      unsigned int np;
      unsigned int tp;
  };

  /***************************/
  /****NHM********************/
  /***************************/
  extern bool debug;

  /***************************/
  /****NHM SYSFS**************/
  /***************************/
  struct nhm_sysfs_io_s {
      ssize_t (*show)(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
      ssize_t (*store)(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);
  };

  struct nhm_sysfs_s {
      struct nhm_sysfs_io_s list;
      struct nhm_sysfs_io_s add;
      struct nhm_sysfs_io_s del;
      struct nhm_sysfs_io_s config;
      struct nhm_sysfs_io_s help;
  };
 
  /**
   * @fn int nhm_sysfs_link(const char* name, struct nhm_sysfs_s fct)
   * @brief Link and create the application directory into the /sys folder.
   * @param name The directory name.
   * @param fct The show/store functions.
   * @return 0 on success else ENOMEM
   */
  int nhm_sysfs_link(const char* name, struct nhm_sysfs_s fct);

  /**
   * @fn void nhm_sysfs_unlink(void)
   * @brief Unlink and remove the application directory into the /sys folder.
   */
  void nhm_sysfs_unlink(void);


  /***************************/
  /****NHM NET****************/
  /***************************/
  /**
   * @fn void nhm_net_start(void)
   * @brief Start the netfilter hook.
   */
  void nhm_net_start(void);

  /**
   * @fn void nhm_net_stop(void)
   * @brief Stop the netfilter hook.
   */
  void nhm_net_stop(void);

#endif /* __NHM_COMMON_H__ */
