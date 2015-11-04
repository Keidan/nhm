#ifndef __NHM_COMMON_H__
  #define __NHM_COMMON_H__


  #include <linux/string.h>       /* Needed by strings operations */
  #include <linux/list.h>

  #define LEN_HW 18
  #define LEN_IPv4 16
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
