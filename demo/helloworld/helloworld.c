/**
 * @file helloworld.c
 * @author Keidan
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3
 *
 *   ___ ___         .__  .__
 *  /   |   \   ____ |  | |  |   ____
 * /    ~    \_/ __ \|  | |  |  /  _ \
 * \    Y    /\  ___/|  |_|  |_(  <_> )
 *  \___|_  /  \___  >____/____/\____/
 *        \/       \/
 *                     .__       .___
 * __  _  _____________|  |    __| _/
 * \ \/ \/ /  _ \_  __ \  |   / __ |
 *  \     (  <_> )  | \/  |__/ /_/ |
 *   \/\_/ \____/|__|  |____/\____ |
 *                                \/
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <nhm.h>


#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int main(int argc, char** argv)
{
  (void) argc;
  (void) argv;
  
  int i, ret, fd, length;
  unsigned char buffer [4];
  struct nhm_s rule;

  printf("Open the connection between the the application and the device...\n");
  fd = nhm_open();
  if (fd < 0)
  {
    perror("Failed to open the device...");
    return errno;
  }
  printf("Connection established between the application and the device\n");

  /* reset the rule */
  nhm_init_rule(&rule);
  /* drop all packet from/to 192.168.0.1 and port 80 */
  rule.ip4 = nhm_to_ipv4(192, 168, 0, 1);
  rule.nf_type = NHM_NF_TYPE_DROP;
  rule.port[0] = 80;
  /* add the rule */
  ret = nhm_add_rule(fd, &rule);
  printf("Result: %d-%d %s\n", ret, errno, strerror(errno));
  /* close the ports 22 to 130*/
  nhm_init_rule(&rule);
  rule.nf_type = NHM_NF_TYPE_DROP;
  rule.port[0] = 22;
  rule.port[1] = 130;
  /* add the rule */
  ret = nhm_add_rule(fd, &rule);
  printf("Result: %d-%d %s\n", ret, errno, strerror(errno));

  /* number of rules */
  ret = nhm_rules_size(fd, &length);

  /* print the rules from the LKM */
  for(i = 0; i < length; i++)
  {
    /* Read the response from the LKM */
    ret = nhm_get_rule(fd, &rule);
    if (ret < 0)
    {
      nhm_close(fd);
      perror("Failed to read the message from the device.");
      return errno;
    }

    printf("%d -> dev:%s\n", i, rule.dev);
    if(!nhm_is_mac_any(rule.hw))
      printf("%d -> hw: %02x:%02x:%02x:%02x:%02x:%02x\n", i, rule.hw[0], rule.hw[1], rule.hw[2], rule.hw[3], rule.hw[4], rule.hw[5]);
    else
      printf("%d -> hw: ANY\n", i);
    if(!nhm_is_ipv4_any(rule.ip4))
    {
      nhm_from_ipv4(buffer, 0, rule.ip4);
      printf("%d -> ip4: %d.%d.%d.%d\n", i, buffer[0], buffer[1], buffer[2], buffer[3]);
    }
    else
    {
      printf("%d -> ip4: ANY\n", i);
    }
    printf("%d -> prt: %d-%d\n", i, rule.port[0], rule.port[1]);
    printf("%d -> eth_proto: %d\n", i, rule.eth_proto);
    printf("%d -> ip_proto: %d\n", i, rule.ip_proto);
    sleep(1);
  }

  sleep(5);
  /* remove all rules */
  ret = nhm_clear_rules(fd);
  printf("Result: %d-%d %s\n", ret, errno, strerror(errno));

  nhm_close(fd);
  return 0;
}
