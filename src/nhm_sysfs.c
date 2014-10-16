#include "nhm_common.h"

#define ATTRB_FLAG_LIST   (1 << 1)
#define ATTRB_FLAG_DEL    (1 << 2)
#define ATTRB_FLAG_ADD    (1 << 3)
#define ATTRB_FLAG_CONFIG (1 << 4)
#define ATTRB_FLAG_HELP   (1 << 5)

static struct kobject *kobj = NULL;
static int attrb_flags = 0;
static struct kobj_attribute attrb_list = __ATTR(list, 0666, NULL, NULL);
static struct kobj_attribute attrb_del = __ATTR(del, 0666, NULL, NULL);
static struct kobj_attribute attrb_add = __ATTR(add, 0666, NULL, NULL);
static struct kobj_attribute attrb_config = __ATTR(config, 0666, NULL, NULL);
static struct kobj_attribute attrb_help = __ATTR(help, 0666, NULL, NULL);

EXPORT_SYMBOL(nhm_sysfs_link);
EXPORT_SYMBOL(nhm_sysfs_unlink);

static int nhm_sysfs_manage_file(struct kobj_attribute *attrb, const struct nhm_sysfs_io_s *fct, int flag, bool create) {
  int ret;
  if(create) {
    /* Set the callbacks */
    attrb->show = fct->show;
    attrb->store = fct->store;
    /* Create the files and set a flag */
    if((ret = sysfs_create_file(kobj, &attrb->attr))) {
      nhm_sysfs_unlink();
      return ret;
    }
    attrb_flags |= flag;
  } else {
    /* Release file */
    if(attrb_flags & flag) {
      attrb_flags &= ~flag;
      sysfs_remove_file(kobj, &attrb->attr);
    }
  }
  return 0;
}

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
  if((ret = nhm_sysfs_manage_file(&attrb_list, &fct.list, ATTRB_FLAG_LIST, 1)))
    return ret;
  if((ret = nhm_sysfs_manage_file(&attrb_add, &fct.add, ATTRB_FLAG_ADD, 1)))
    return ret;
  if((ret = nhm_sysfs_manage_file(&attrb_del, &fct.del, ATTRB_FLAG_DEL, 1)))
    return ret;
  if((ret = nhm_sysfs_manage_file(&attrb_config, &fct.config, ATTRB_FLAG_CONFIG, 1)))
    return ret;
  if((ret = nhm_sysfs_manage_file(&attrb_help, &fct.help, ATTRB_FLAG_HELP, 1)))
    return ret;
  return 0;
}

/**
 * @fn void nhm_sysfs_unlink(void)
 * @brief Unlink and remove the application directory into the /sys folder.
 */
void nhm_sysfs_unlink(void) {
  if(kobj) {
    /* Release all files */
    nhm_sysfs_manage_file(&attrb_list, NULL, ATTRB_FLAG_LIST, 0);
    nhm_sysfs_manage_file(&attrb_add, NULL, ATTRB_FLAG_ADD, 0);
    nhm_sysfs_manage_file(&attrb_del, NULL, ATTRB_FLAG_DEL, 0);
    nhm_sysfs_manage_file(&attrb_config, NULL, ATTRB_FLAG_CONFIG, 0);
    nhm_sysfs_manage_file(&attrb_help, NULL, ATTRB_FLAG_HELP, 0);
    kobject_put(kobj);
    kobj = NULL;
  }
}

