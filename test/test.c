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
  fd = open("/dev/nhm", O_RDWR);
  if (fd < 0){
    perror("Failed to open the device...");
    return errno;
  }

  memset(&message, 0, NHM_LENGTH);
  message.s_ip4 = nhm_to_ip(192, 168, 0, 1);
  
  nhm_from_ip(buffer, 0, message.s_ip4);
  printf("Result: %d\n", message.s_ip4);
  printf("Result: %d.%d.%d.%d\n", buffer[3], buffer[2], buffer[1], buffer[0]);

  ret = ioctl(fd, NHM_IOCTL_ADD, &message);
  printf("Result: %d-%d %s\n", ret, errno, strerror(errno));

  message.s_ip4 = nhm_to_ip(192, 168, 0, 2);
  ret = ioctl(fd, NHM_IOCTL_ADD, &message);
  printf("Result: %d-%d %s\n", ret, errno, strerror(errno));

  ret = ioctl(fd, NHM_IOCTL_LENGTH, &length);
  printf("Length: %d\n", length);

  message.s_ip4 = nhm_to_ip(192, 168, 0, 2);
  ret = ioctl(fd, NHM_IOCTL_DEL, &message);
  printf("Result: %d-%d %s\n", ret, errno, strerror(errno));

  message.s_ip4 = nhm_to_ip(192, 168, 0, 2);
  ret = ioctl(fd, NHM_IOCTL_DEL, &message);
  printf("Result: %d-%d %s\n", ret, errno, strerror(errno));

  ret = ioctl(fd, NHM_IOCTL_LENGTH, &length);
  printf("Length: %d\n", length);

  ret = ioctl(fd, NHM_IOCTL_CLEAR, NULL);
  printf("Result: %d-%d %s\n", ret, errno, strerror(errno));

  ret = ioctl(fd, NHM_IOCTL_LENGTH, &length);
  printf("Length: %d\n", length);
  printf("Result: %d-%d %s\n", ret, errno, strerror(errno));



  message.s_ip4 = nhm_to_ip(192, 168, 0, 1);
  ret = ioctl(fd, NHM_IOCTL_ADD, &message);
  printf("Result: %d-%d %s\n", ret, errno, strerror(errno));
  message.s_ip4 = nhm_to_ip(192, 168, 0, 2);
  ret = ioctl(fd, NHM_IOCTL_ADD, &message);
  printf("Result: %d-%d %s\n", ret, errno, strerror(errno));
  ret = ioctl(fd, NHM_IOCTL_LENGTH, &length);
  printf("Result: %d-%d %s\n", ret, errno, strerror(errno));
  printf("Length: %d\n", length);
  message.s_ip4 = nhm_to_ip(192, 168, 0, 3);
  ret = ioctl(fd, NHM_IOCTL_ADD, &message);
  printf("Result: %d-%d %s\n", ret, errno, strerror(errno));
  ret = ioctl(fd, NHM_IOCTL_LENGTH, &length);
  printf("Result: %d-%d %s\n", ret, errno, strerror(errno));
  printf("Length: %d\n", length);


  for(i = 0; ; i++) {
    ret = read(fd, &message, NHM_LENGTH);        // Read the response from the LKM
    if (ret < 0){
      perror("Failed to read the message from the device.");
      return errno;
    }

    printf("%d -> s_hw: %02x:%02x:%02x:%02x:%02x:%02x\n", i, message.s_hw[0], message.s_hw[1], message.s_hw[2], message.s_hw[3], message.s_hw[4], message.s_hw[5]);
    printf("%d -> d_hw: %02x:%02x:%02x:%02x:%02x:%02x\n", i, message.d_hw[0], message.d_hw[1], message.d_hw[2], message.d_hw[3], message.d_hw[4], message.d_hw[5]);
    printf("%d -> s_ip4: %d\n", i, message.s_ip4);
    printf("%d -> d_ip4: %d\n", i, message.d_ip4);
    printf("%d -> s_prt: %d-%d\n", i, message.s_port[0], message.s_port[1]);
    printf("%d -> d_prt: %d-%d\n",  i, message.d_port[0], message.d_port[1]);
    printf("%d -> eth_proto: %d\n", i, message.eth_proto);
    printf("%d -> ip_proto: %d\n", i, message.ip_proto);
    ret = ioctl(fd, NHM_IOCTL_ZERO, NULL);
    printf("Result: %d-%d %s\n", ret, errno, strerror(errno));
    sleep(1);
  }

  /*printf("Reading from the device...\n");
  ret = read(fd, &message, NHM_LENGTH);        // Read the response from the LKM
  if (ret ret = read(fd, &message, NHM_LENGTH);        // Read the response from the LKM
  if (ret < 0){
    perror("Failed to read the message from the device.");
    return errno;
  } < 0){
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
  printf("ip_proto: %d\n", message.ip_proto);*/

  close(fd);
  return 0;
}
