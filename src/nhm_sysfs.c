#include "nhm_sysfs.h"

#define ATTRB_FLAG_DEBUG  (1 << 1)
#define ATTRB_FLAG_LIST   (1 << 2)
#define ATTRB_FLAG_DEL    (1 << 3)
#define ATTRB_FLAG_ADD    (1 << 4)
#define ATTRB_FLAG_CONFIG (1 << 5)

static struct kobject *kobj = NULL;
static int attrb_flags = 0;
static struct kobj_attribute attrb_debug = __ATTR(debug, 0666, NULL, NULL);
static struct kobj_attribute attrb_list = __ATTR(list, 0666, NULL, NULL);
static struct kobj_attribute attrb_del = __ATTR(del, 0666, NULL, NULL);
static struct kobj_attribute attrb_add = __ATTR(add, 0666, NULL, NULL);
static struct kobj_attribute attrb_config = __ATTR(config, 0666, NULL, NULL);

EXPORT_SYMBOL(nhm_sysfs_link);
EXPORT_SYMBOL(nhm_sysfs_unlink);


/**
 * @fn int nhm_sysfs_link(const char* name, struct nhm_sysfs_s fct)
 * @brief Link and create the application directory into the /sys folder.
 * @param name The directory name.
 * @param fct The show/store functions.
 * @return 0 on success else ENOMEM
 */
int nhm_sysfs_link(const char* name, struct nhm_sysfs_s fct) {
  int ret;
  /* create a dir in sys/ */
  kobj = kobject_create_and_add(name, NULL);
  if (!kobj) return -ENOMEM;
  /* Set the callbacks */
  attrb_debug.show = fct.debug.show;
  attrb_debug.store = fct.debug.store;
  attrb_list.show = fct.list.show;
  attrb_list.store = fct.list.store;
  attrb_add.show = fct.add.show;
  attrb_add.store = fct.add.store;
  attrb_del.show = fct.del.show;
  attrb_del.store = fct.del.store;
  attrb_config.show = fct.config.show;
  attrb_config.store = fct.config.store;
  /* Create the files and set a flag */
  attrb_flags = 0;
  if((ret = sysfs_create_file(kobj, &attrb_debug.attr))) {
    nhm_sysfs_unlink();
    return ret;
  }
  attrb_flags |= ATTRB_FLAG_DEBUG;
  if((ret = sysfs_create_file(kobj, &attrb_list.attr))) {
    nhm_sysfs_unlink();
    return ret;
  }
  attrb_flags |= ATTRB_FLAG_LIST;
  if((ret = sysfs_create_file(kobj, &attrb_add.attr))) {
    nhm_sysfs_unlink();
    return ret;
  }
  attrb_flags |= ATTRB_FLAG_DEL;
  if((ret = sysfs_create_file(kobj, &attrb_del.attr))) {
    nhm_sysfs_unlink();
    return ret;
  }
  attrb_flags |= ATTRB_FLAG_ADD;
  if((ret = sysfs_create_file(kobj, &attrb_config.attr))) {
    nhm_sysfs_unlink();
    return ret;
  }
  attrb_flags |= ATTRB_FLAG_CONFIG;
  return 0;
}

/**
 * @fn void nhm_sysfs_unlink(void)
 * @brief Unlink and remove the application directory into the /sys folder.
 */
void nhm_sysfs_unlink(void) {
  if(kobj) {
    kobject_put(kobj);
    kobj = NULL;
    /* Release all files */
    if(attrb_flags & ATTRB_FLAG_DEBUG) {
      attrb_flags &= ~ATTRB_FLAG_DEBUG;
      sysfs_remove_file(kobj, &attrb_debug.attr);
    }
    if(attrb_flags & ATTRB_FLAG_LIST) {
      attrb_flags &= ~ATTRB_FLAG_LIST;
      sysfs_remove_file(kobj, &attrb_list.attr);
    }
    if(attrb_flags & ATTRB_FLAG_ADD) {
      attrb_flags &= ~ATTRB_FLAG_ADD;
      sysfs_remove_file(kobj, &attrb_add.attr);
    }
    if(attrb_flags & ATTRB_FLAG_DEL) {
      attrb_flags &= ~ATTRB_FLAG_DEL;
      sysfs_remove_file(kobj, &attrb_del.attr);
    }
    if(attrb_flags & ATTRB_FLAG_CONFIG) {
      attrb_flags &= ~ATTRB_FLAG_CONFIG;
      sysfs_remove_file(kobj, &attrb_config.attr);
    }
  }
}

