#ifndef __NHM_SYSFS_H__
  #define __NHM_SYSFS_H__


  #include <linux/kobject.h>      /* Needed by kobject */
  #include <linux/sysfs.h>        /* Needed by sysfs_xxxx */
  #include <linux/string.h>       /* Needed by strings operations */



  struct nhm_sysfs_io_s {
      ssize_t (*show)(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
      ssize_t (*store)(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);
  };

  struct nhm_sysfs_s {
      struct nhm_sysfs_io_s debug;
      struct nhm_sysfs_io_s list;
      struct nhm_sysfs_io_s add;
      struct nhm_sysfs_io_s del;
      struct nhm_sysfs_io_s config;
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

#endif /* __NHM_SYSFS_H__ */
