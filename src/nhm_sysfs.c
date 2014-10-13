#include "nhm_sysfs.h"



static struct kobject *kobj;


EXPORT_SYMBOL(nhm_sysfs_link);
EXPORT_SYMBOL(nhm_sysfs_unlink);
EXPORT_SYMBOL(nhm_sysfs_add_file);
EXPORT_SYMBOL(nhm_sysfs_remove_file);

/**
 * @fn int nhm_sysfs_link(const char* name)
 * @brief Link and create the application directory into the /sys folder.
 * @param name The directory name.
 * @return 0 on success else ENOMEM
 */
int nhm_sysfs_link(const char* name) {
  /* create a dir in sys/ */
  kobj = kobject_create_and_add(name, NULL);
  if (!kobj) return -ENOMEM;
  return 0;
}

/**
 * @fn void nhm_sysfs_unlink(void)
 * @brief Unlink and remove the application directory into the /sys folder.
 */
void nhm_sysfs_unlink(void) {
  kobject_put(kobj);
}

/**
 * @fn int nhm_sysfs_add_file(struct kobj_attribute attrb)
 * @brief Create a new file into the linked directory.
 * @param attrb File attribute.
 * @return 0 on success else the error code.
 */
int nhm_sysfs_add_file(struct kobj_attribute attrb) {
  /* create a attribute file in nhm */
  return sysfs_create_file(kobj, &attrb.attr);
}

/**
 * @fn void nhm_sysfs_remove_file(struct kobj_attribute attrb)
 * @brief Remove a file from the linked directory.
 * @param attrb File attribute.
 */
void nhm_sysfs_remove_file(struct kobj_attribute attrb) {
  sysfs_remove_file(kobj, &attrb.attr);
}
