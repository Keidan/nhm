#ifndef __NHM_COMMON_H__
  #define __NHM_COMMON_H__


  #include <linux/string.h>       /* Needed by strings operations */
  #include <linux/list.h>
  #include <nhm.h>


  struct nhm_net_entry_s {
      struct nhm_s entry;
      struct list_head list;
  };

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
