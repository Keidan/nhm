#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/init.h>		/* Needed for the macros */
#include "nhm_common.h"

/**
 * @fn bool to_uint(char *value, size_t length, unsigned int *p_uint)
 * @brief Convert a string int to int.
 * @param value The string value.
 * @param length The string value length.
 * @param p_uint The output pointer.
 * @return true on success, else false.
 */
bool to_uint(char *value, size_t length, unsigned int *p_uint) {
  char str[6];
  int ret;
  /* temp copy */
  memset(str, 0, 6);
  strncpy(str, value, length);
  /* int convert */
  ret = kstrtouint(str, 10, p_uint);
  if((ret == -ERANGE) || (ret == -EINVAL)) return false;
  return true;
}

/**
 * @fn bool decode_args(const char* buf, size_t count, struct nhm_net_entry_s* entry)
 * @brief Decode the arguments
 * @param buf The arguments buffer.
 * @param count The buffer size.
 * @param entry The output entry.
 * @return false if the decoding failed else true
 */
bool decode_args(const char* buf, size_t count, struct nhm_net_entry_s* entry) {
  size_t length;
  char *value, *copy = (char*)buf + count;
  int last_space = count, diff = 0;

  if(count > MIN_STR_RULE_LEN) {
    memset(&entry, 0, sizeof(struct nhm_net_entry_s));
    while(copy != (buf - 1)) {
      if(copy[0] == ' ' || copy == buf) {
	/* argument length */
	length = last_space - (copy - buf);
	/* trim spaces */
	value = copy[0] == ' ' ? copy + 1 : copy;
	if(value[length - 1] == ' ') length--;

	if(!memcmp(value, "sh=", MIN_STR_RULE_LEN)) {
	  strncpy(entry->sh, value + MIN_STR_RULE_LEN, length - MIN_STR_RULE_LEN);
	} else if(!memcmp(value, "dh=", MIN_STR_RULE_LEN)) {
	  strncpy(entry->dh, value + MIN_STR_RULE_LEN, length - MIN_STR_RULE_LEN);
	} else if(!memcmp(value, "si=", MIN_STR_RULE_LEN)) {
	  strncpy(entry->si, value + MIN_STR_RULE_LEN, length - MIN_STR_RULE_LEN);
	} else if(!memcmp(value, "di=", MIN_STR_RULE_LEN)) {
	  strncpy(entry->di, value + MIN_STR_RULE_LEN, length - MIN_STR_RULE_LEN);
	} else if(!memcmp(value, "sp1=", MIN_PORT_RULE_LEN)) {
	  to_uint(value + MIN_PORT_RULE_LEN, length - MIN_PORT_RULE_LEN, (unsigned int*)&entry->sp[0]);
	} else if(!memcmp(value, "sp2=", MIN_PORT_RULE_LEN)) {
	  to_uint(value + MIN_PORT_RULE_LEN, length - MIN_PORT_RULE_LEN, (unsigned int*)&entry->sp[1]);
	} else if(!memcmp(value, "dp1=", MIN_PORT_RULE_LEN)) {
	  to_uint(value + MIN_PORT_RULE_LEN, length - MIN_PORT_RULE_LEN, (unsigned int*)&entry->dp[0]);
	} else if(!memcmp(value, "dp2=", MIN_PORT_RULE_LEN)) {
	  to_uint(value + MIN_PORT_RULE_LEN, length - MIN_PORT_RULE_LEN, (unsigned int*)&entry->dp[1]);
	} else if(!memcmp(value, "np=", MIN_STR_RULE_LEN)) {
	  to_uint(value + MIN_STR_RULE_LEN, length - MIN_STR_RULE_LEN, &entry->np);
	} else if(!memcmp(value, "tp=", MIN_STR_RULE_LEN)) {
	  to_uint(value + MIN_STR_RULE_LEN, length - MIN_STR_RULE_LEN, &entry->tp);
	}
	diff = (int)(copy - buf);
	if(diff != -1) last_space = diff;
      }
      copy--;
    }
    return true;
  }
  return false;
}
