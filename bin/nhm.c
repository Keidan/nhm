/**
*******************************************************************************
* @file nhm.c
* @author Keidan
* @par Project nhm
* @copyright Copyright 2015 Keidan, all right reserved.
* @par License:
* This software is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY.
*
* Licence summary : 
*    You can modify and redistribute the sources code and binaries.
*    You can send me the bug-fix
*
* Term of the licence in in the file licence.txt.
*
*  _______          __                       __    
*  \      \   _____/  |___  _  _____________|  | __
*  /   |   \_/ __ \   __\ \/ \/ /  _ \_  __ \  |/ /
* /    |    \  ___/|  |  \     (  <_> )  | \/    < 
* \____|__  /\___  >__|   \/\_/ \____/|__|  |__|_ \
*         \/     \/                              \/
*   ___ ___                __                      
*  /   |   \  ____   ____ |  | __                  
* /    ~    \/  _ \ /  _ \|  |/ /                  
* \    Y    (  <_> |  <_> )    <                   
*  \___|_  / \____/ \____/|__|_ \                  
*        \/                    \/                  
* __________.__                            
* \______   \__| ____ _____ _______ ___.__.
*  |    |  _/  |/    \\__  \\_  __ <   |  |
*  |    |   \  |   |  \/ __ \|  | \/\___  |
*  |______  /__|___|  (____  /__|   / ____|
*         \/        \/     \/       \/     
*******************************************************************************
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
#include <gtk/gtk.h>

static struct nhm_s message;
 
int main(int argc, char** argv){
  int i, ret, fd, length;
  struct sockaddr_in n;
  unsigned char buffer [4];
  GtkWidget* window;

  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "NHM Manager");
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    g_signal_connect(G_OBJECT(window), "delete-event", G_CALLBACK(gtk_main_quit), NULL);
  gtk_widget_show_all(window);

  /*
  printf("Open the connection between the the application and the device...\n");
  fd = nhm_open();
  if (fd < 0){
    perror("Failed to open the device...");
    return errno;
  }
  printf("Connection established between theapplication and the device\n");

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
  */

  gtk_main();
  return EXIT_SUCCESS;
}
