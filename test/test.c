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


static struct nhm_s message;
 
int main(){
  int i, ret, fd, length;
  struct sockaddr_in n;
  unsigned char buffer [4];
  printf("Starting device test code example...\n");
  fd = nhm_open();
  if (fd < 0){
    perror("Failed to open the device...");
    return errno;
  }

  nhm_init_rule(&message);
  message.ip4 = nhm_to_ipv4(192, 168, 0, 1);
  
  nhm_from_ipv4(buffer, 0, message.ip4);
  printf("Result: %d\n", message.ip4);
  printf("Result: %d.%d.%d.%d\n", buffer[3], buffer[2], buffer[1], buffer[0]);

  ret = nhm_add_rule(fd, &message);
  printf("Result: %d-%d %s\n", ret, errno, strerror(errno));

  message.ip4 = nhm_to_ipv4(192, 168, 0, 2);
  ret = nhm_add_rule(fd, &message);
  printf("Result: %d-%d %s\n", ret, errno, strerror(errno));

  memset(&message, 0, NHM_LENGTH);
  message.nf_type = NHM_NF_TYPE_DROP;
  message.port[0] = 80;
  ret = nhm_add_rule(fd, &message);
  printf("Result: %d-%d %s\n", ret, errno, strerror(errno));

  ret = nhm_rules_size(fd, &length);
  printf("Length: %d\n", length);


  for(i = 0; i < length; i++) {
    ret = nhm_get_rule(fd, &message);        // Read the response from the LKM
    if (ret < 0){
      perror("Failed to read the message from the device.");
      return errno;
    }

    printf("%d -> dev:%s\n", i, message.dev);
    printf("%d -> hw: %02x:%02x:%02x:%02x:%02x:%02x\n", i, message.hw[0], message.hw[1], message.hw[2], message.hw[3], message.hw[4], message.hw[5]);
    printf("%d -> ip4: %d\n", i, message.ip4);
    printf("%d -> prt: %d-%d\n", i, message.port[0], message.port[1]);
    printf("%d -> eth_proto: %d\n", i, message.eth_proto);
    printf("%d -> ip_proto: %d\n", i, message.ip_proto);
    sleep(1);
  }

  nhm_close(fd);
  for(;;) sleep(1);
  return 0;
}
