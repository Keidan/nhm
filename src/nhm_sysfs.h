#ifndef __NHM_SYSFS_H__
  #define __NHM_SYSFS_H__


  #include <linux/kobject.h>      /* Needed by kobject */
  #include <linux/sysfs.h>        /* Needed by sysfs_xxxx */
  #include <linux/string.h>       /* Needed by strings operations */

  /**
   * @fn int nhm_sysfs_link(const char* name)
   * @brief Link and create the application directory into the /sys folder.
   * @param name The directory name.
   * @return 0 on success else ENOMEM
   */
  int nhm_sysfs_link(const char* name);

  /**
   * @fn void nhm_sysfs_unlink(void)
   * @brief Unlink and remove the application directory into the /sys folder.
   */
  void nhm_sysfs_unlink(void);

  /**
   * @fn int nhm_sysfs_add_file(struct kobj_attribute attrb)
   * @brief Create a new file into the linked directory.
   * @param attrb File attribute.
   * @return 0 on success else the error code.
   */
  int nhm_sysfs_add_file(struct kobj_attribute attrb);

  /**
   * @fn void nhm_sysfs_remove_file(struct kobj_attribute attrb)
   * @brief Remove a file from the linked directory.
   * @param attrb File attribute.
   */
  void nhm_sysfs_remove_file(struct kobj_attribute attrb);

#endif /* __NHM_SYSFS_H__ */
