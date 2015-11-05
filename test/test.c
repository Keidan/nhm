#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <nhm.h>


static struct nhm_s message;
 
int main(){
  int ret, fd;
  printf("Starting device test code example...\n");
  fd = open("/dev/nhm", O_RDWR);
  if (fd < 0){
    perror("Failed to open the device...");
    return errno;
  }

  memset(&message, 0, NHM_LENGTH);
  message.action = NHM_ACTION_ADD;
  strcpy((char*)message.s_hw, "00:01:02:03:04:05");
  strcpy((char*)message.d_hw, "05:04:03:02:01:00");
  strcpy((char*)message.s_ip4, "192.168.5.1");
  strcpy((char*)message.d_ip4, "192.168.5.5");
  message.s_port[0]=80;

  ret = write(fd, &message, NHM_LENGTH);
  if (ret < 0){
    perror("Failed to write the message to the device.");
    return errno;
  }

  memset(&message, 0, NHM_LENGTH);
  message.action = NHM_ACTION_LIST;
  ret = write(fd, &message, NHM_LENGTH);
  if (ret < 0){
    perror("Failed to write the message to the device.");
    return errno;
  }
 
  printf("Reading from the device...\n");
  ret = read(fd, &message, NHM_LENGTH);        // Read the response from the LKM
  if (ret < 0){
    perror("Failed to read the message from the device.");
    return errno;
  }
  printf("s_hw: %s\n", message.s_hw);
  printf("d_hw: %s\n", message.d_hw);
  printf("s_ip4: %s\n", message.s_ip4);
  printf("d_ip4: %s\n", message.d_ip4);
  printf("s_prt: %d-%d\n", message.s_port[0], message.s_port[1]);
  printf("d_prt: %d-%d\n",  message.d_port[0], message.d_port[1]);
  printf("eth_proto: %d\n", message.eth_proto);
  printf("ip_proto: %d\n", message.ip_proto);

  return 0;
}
