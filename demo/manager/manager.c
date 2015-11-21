/**
*******************************************************************************
* @file manager.c
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
*    _____                                             
*   /     \ _____    ____ _____     ____   ___________ 
*  /  \ /  \\__  \  /    \\__  \   / ___\_/ __ \_  __  \
* /    Y    \/ __ \|   |  \/ __ \_/ /_/  >  ___/|  | \/
* \____|__  (____  /___|  (____  /\___  / \___  >__|   
*         \/     \/     \/     \//_____/      \/       
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
#include <signal.h>
#include <gtk/gtk.h>
#include "manager_ui.h"

static int nhm_fd = -1;

#define THREAD_DELAY_SEC 2





/**
 * @fn static gpointer gtk_thread_refresh_ui_cb(gpointer data)
 * @param data Pointer to globale ctx.
 * @return NULL
 */
static gpointer gtk_thread_refresh_ui_cb(gpointer data);

/**
 * @fn static void sig_function(int sig)
 * @brief sigaction callback.
 * @param sig The signal
 */
static void sig_function(int sig);

/**
 * @fn static void atexit_function(void)
 * @brief atexit callback.
 */
static void atexit_function(void);


 
int main(int argc, char** argv){
  struct sigaction sa;
  struct gtk_ctx_s ctx;

  memset(&ctx, 0, sizeof(struct gtk_ctx_s));
  memset(&sa, 0, sizeof(struct sigaction));
  atexit(atexit_function);
  sa.sa_handler = sig_function;
  (void)sigaction(SIGINT, &sa, NULL);
  (void)sigaction(SIGTERM, &sa, NULL);

  /* initialize the threads stack. */
#if !GLIB_CHECK_VERSION(2,32,0)
  g_thread_init(NULL);
#endif
  /* initialize GDK. */
  gdk_threads_init();
  /* Obtain gtk's global lock */
  gdk_threads_enter();
  /* initialize GTK. */
  gtk_init(&argc, &argv);


  gtk_manager_build_main_ui_and_show(&ctx);

  /* gtk main loop */
  gtk_main();
  gdk_threads_leave();

  return EXIT_SUCCESS;
}

/**
 * @fn static gpointer gtk_thread_refresh_ui_cb(gpointer data)
 * @param data Pointer to globale ctx.
 * @return NULL
 */
static gpointer gtk_thread_refresh_ui_cb(gpointer data) {
  struct gtk_ctx_s *ctx = (struct gtk_ctx_s*)data;
  GtkTreeIter iter;
  int i, ret, length;
  unsigned char buffer [4];
  struct nhm_s rule;
  GString *str = NULL, *str_proto, *str_hw, *str_ip, *str_type;
  
  for(;!ctx->end;) {
    gdk_threads_enter();
    gtk_list_store_clear(ctx->listStore);
    if(nhm_fd != -1) {
      /* number of rules */
      ret = nhm_rules_size(nhm_fd, &length);
      if(ret == -1) {
        /* Display the error message */
	str = g_string_new("Unable to read the number of rules");
	g_string_append_printf(str, ": %s", strerror(errno));
	gtk_show_msg(ctx, GTK_MESSAGE_ERROR, str->str);
	g_string_free(str, TRUE);
	gdk_threads_leave();
	break;
      }

      /* read the rules from the LKM */
      for(i = 0; i < length; i++) {
	/* Read the response from the LKM */
	ret = nhm_get_rule(nhm_fd, &rule);
	if (ret < 0) {
	  /* Display the error message */
	  str = g_string_new("Failed to read the message from the module.");
	  g_string_append_printf(str, ": %s", strerror(errno));
	  gtk_show_msg(ctx, GTK_MESSAGE_ERROR, str->str);
	  g_string_free(str, TRUE);
	  gdk_threads_leave();
	  break;
	}
	str_proto = g_string_sized_new(18);
	str_hw = g_string_sized_new(18);
	str_ip = g_string_sized_new(30);
	str_type = g_string_sized_new(30);
	g_string_append_printf(str_hw, "%02x:%02x:%02x:%02x:%02x:%02x", rule.hw[0], rule.hw[1], rule.hw[2], rule.hw[3], rule.hw[4], rule.hw[5]);
	if(rule.ip4) {
	  nhm_from_ipv4(buffer, 0, rule.ip4);
	  g_string_append_printf(str_ip, "%d.%d.%d.%d", buffer[0], buffer[1], buffer[2], buffer[3]);
	  if(!rule.port[1])
	    g_string_append_printf(str_ip, ":%d", rule.port[0]);
	  else
	    g_string_append_printf(str_ip, ":[%d-%d]", rule.port[0], rule.port[1]);
	}
	if(!str_ip->str[0])
	  g_string_append(str_ip, "unknown");
	
	g_string_append_printf(str_proto, "%d-%d", rule.eth_proto, rule.ip_proto);


	gtk_list_store_append(ctx->listStore, &iter);
	gtk_list_store_set(ctx->listStore, &iter, 
			   0, rule.dev[0] ? "All" : rule.dev, 
			   1, str_type->str,
			   2, (rule.dir == NHM_DIR_INPUT ? "input" : (rule.dir == NHM_DIR_OUTPUT ? "output" : "both")), 
			   3, str_proto->str,  
			   4, str_hw->str, 
			   5, str_ip->str, 
			   6, rule.applied, 
			   -1);
	g_string_free(str_proto, TRUE);
	g_string_free(str_hw, TRUE);
	g_string_free(str_ip, TRUE);
	g_string_free(str_type, TRUE);
      } 
      gdk_threads_leave();
      sleep(THREAD_DELAY_SEC);
    }
  }
  /* Rebuild the UI state*/
  gdk_threads_enter();
  gtk_widget_set_sensitive(ctx->buttonConnect, TRUE);
  gtk_button_set_label(GTK_BUTTON(ctx->buttonConnect), "Connect");
  gtk_widget_set_sensitive(ctx->buttonAdd, FALSE);
  gtk_widget_set_sensitive(ctx->buttonRemove, FALSE);
  gtk_widget_set_sensitive(ctx->listView, FALSE);
  /* close the device */
  if(nhm_fd != -1) nhm_close(nhm_fd), nhm_fd = -1;
  gtk_list_store_clear(ctx->listStore);
  gdk_threads_leave();
#if GLIB_CHECK_VERSION(2,32,0)
  g_thread_unref(ctx->thread);
#endif
  return NULL;
}
/**
 * @fn static void atexit_function(void)
 * @brief atexit callback.
 */
static void atexit_function(void) { 
  if(nhm_fd != -1) nhm_close(nhm_fd), nhm_fd = -1;
}

/**
 * @fn static void sig_function(int sig)
 * @brief sigaction callback.
 * @param sig The signal
 */
static void sig_function(int sig) {
  exit(sig);
}


/**
 * @fn void gtk_global_button_clicked(GtkWidget* button, gpointer p_data)
 * @brief Main callback for the buttons signal.
 * @param button Owner button.
 * @param p_data p_data
 */
void gtk_global_button_clicked(GtkWidget* button, gpointer p_data) {
  struct gtk_ctx_s *ctx = (struct gtk_ctx_s*)p_data;
  GString* str = NULL;
  GError *error = NULL;
  if(button == ctx->buttonConnect) {
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button))) {
      ctx->end = 0;
      /* open the connection with the module */
      nhm_fd = nhm_open();
      if(nhm_fd == -1) {
	/* Display the error message */
	str = g_string_new(NHM_DEVICE_PATH);
	g_string_append_printf(str, ": %s", strerror(errno));
	gtk_show_msg(ctx, GTK_MESSAGE_ERROR, str->str);
	g_string_free(str, TRUE);
	/* Restore the UI changes */
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), FALSE);
	gtk_widget_set_sensitive(button, TRUE);
	return;
      }
      /* Create new thread */
#if !GLIB_CHECK_VERSION(2,32,0)
      ctx->thread = g_thread_create(gtk_thread_refresh_ui_cb, (gpointer)ctx, FALSE, &error);
#else
      ctx->thread = g_thread_try_new("RefreshUI", gtk_thread_refresh_ui_cb, (gpointer)ctx, &error);
#endif
      if(!ctx->thread) {
	g_print( "Error: %s\n", error->message);
	gtk_show_msg(ctx, GTK_MESSAGE_ERROR, error->message);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(button), FALSE);
	return;
      }
      gtk_button_set_label(GTK_BUTTON(button), "Disconnect");
      gtk_widget_set_sensitive(ctx->buttonAdd, TRUE);
      gtk_widget_set_sensitive(ctx->buttonRemove, TRUE);
      gtk_widget_set_sensitive(ctx->listView, TRUE);
    }
    else {
      gtk_widget_set_sensitive(ctx->buttonConnect, FALSE);
      ctx->end = 1;
    }
  } else if(button == ctx->buttonRemove) {
    gtk_tree_view_remove_selected_items(GTK_TREE_VIEW(ctx->listView));
  }
}

