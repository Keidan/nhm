#ifndef __NHM_H__
  #define __NHM_H__

  #define LEN_HW 18
  #define LEN_IPv4 16
  #define LEN_PORTS 2
  #define NHM_LENGTH sizeof(struct nhm_s)

  typedef enum {
    NHM_ACTION_LIST=0,
    NHM_ACTION_ADD,
    NHM_ACTION_DEL,
    NHM_ACTION_CLEAR
  } nhm_action_te;

  struct nhm_s {
      nhm_action_te  action;
      unsigned char  s_hw[LEN_HW];
      unsigned char  d_hw[LEN_HW];
      unsigned char  s_ip4[LEN_IPv4];
      unsigned char  d_ip4[LEN_IPv4];
      unsigned short s_port[LEN_PORTS];
      unsigned short d_port[LEN_PORTS];
      unsigned short eth_proto;
      unsigned short ip_proto;
  };

#endif /* __NHM_H__ */
