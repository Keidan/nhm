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

struct gtk_ctx_s {
    GtkWidget* window;
    GtkStatusIcon *statusIcon;
    GtkWidget* menu;
    GtkWidget* menuItemView;
    GtkWidget* menuItemExit;
    GtkWidget* menuItemSeparator;
    GtkWidget* buttonAdd;
    GtkWidget* buttonRemove;
    GtkWidget* buttonConnect;
    GtkWidget *listView;
    GtkListStore *listStore;
    GThread *thread;
    char end;
};


static char* columns_header[] = {
  "Device", "Type", "Dir", "Proto", "HW", "IP", "Applied"
};
static int nhm_fd = -1;

#define COLUMNS_HEADER_LENGTH (sizeof(columns_header) / sizeof(columns_header[0]))
#define THREAD_DELAY_SEC 2



/**
 * @fn static void gtk_status_icon_activate (GtkStatusIcon * status_icon, gpointer p_data)
 * @brief Display or hide the main window (view menu action).
 * @param status_icon status_icon
 * @param p_data p_data
 */
static void gtk_status_icon_activate(GtkStatusIcon * pStatusIcon, gpointer p_data);

/**
 * @fn static void gtk_status_icon_popup(GtkStatusIcon *status_icon, guint button, guint32 activate_time, gpointer p_data)
 * @brief Display the popupmenu when the user made a right click on the systray icon.
 * @param status_icon status_icon
 * @param button button
 * @param activate_time activate_time
 * @param p_data p_data
 */
static void gtk_status_icon_popup(GtkStatusIcon *status_icon, guint button, guint32 activate_time, gpointer p_data);

/**
 * @fn static void gtk_global_button_clicked(GtkWidget *button, gpointer p_data)
 * @brief Main callback for the buttons signal.
 * @param button Owner button.
 * @param p_data p_data
 */
static void gtk_global_button_clicked(GtkWidget *button, gpointer p_data);

/**
 * @fn static void gtk_show_msg(struct gtk_ctx_s *ctx, int type, const char* msg)
 * @brief Show a message.
 * @param ctx The context.
 * @param type The message type (eg: GTK_MESSAGE_INFO).
 * @param msg The message to display
 */
static void gtk_show_msg(struct gtk_ctx_s *ctx, int type, const char* msg);

/**
 * @fn static void gtk_tree_view_remove_selected_items(GtkTreeView *treeview)
 * @brief Remove the selected items from the treeview (I've found this function after a research on google).
 * @param treeview The tree view.
 */
static void gtk_tree_view_remove_selected_items(GtkTreeView *treeview);

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
  GtkCellRenderer *cellRenderer;
  GtkTreeViewColumn* column;
  GtkWidget *scrollbar, *vBox;
  unsigned int i;
  struct gtk_ctx_s ctx;
  GtkWidget* bbox;
  GtkTreeSelection *selection;

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

  /* Create the main window */
  ctx.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(ctx.window), "NHM Manager");
  gtk_window_set_position(GTK_WINDOW(ctx.window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(ctx.window), 500, 300);

  /* create the main container */
  vBox = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(ctx.window), vBox);

  /* Add to the systray */
  ctx.statusIcon = gtk_status_icon_new_from_stock (GTK_STOCK_NETWORK);
  gtk_status_icon_set_tooltip (GTK_STATUS_ICON (ctx.statusIcon), "NHM Manager");

  /* Create the popupmenu used by the systray. */
  ctx.menu = gtk_menu_new();
  ctx.menuItemView = gtk_menu_item_new_with_label ("Hide");
  ctx.menuItemExit = gtk_menu_item_new_with_label ("Exit");
  ctx.menuItemSeparator = gtk_separator_menu_item_new();
 
  /* add the submenu to the popupmenu */
  gtk_menu_shell_append (GTK_MENU_SHELL (ctx.menu), ctx.menuItemView);
  gtk_menu_shell_append (GTK_MENU_SHELL (ctx.menu), ctx.menuItemSeparator);
  gtk_menu_shell_append (GTK_MENU_SHELL (ctx.menu), ctx.menuItemExit);
  gtk_widget_show_all (ctx.menu);

  /* Create the buttons */
  ctx.buttonConnect = gtk_toggle_button_new_with_label("Connect");
  ctx.buttonAdd = gtk_button_new_with_label("Add");
  ctx.buttonRemove = gtk_button_new_with_label("Remove");
  bbox = gtk_hbutton_box_new();
  gtk_container_set_border_width (GTK_CONTAINER (bbox), 5);
  gtk_button_box_set_spacing (GTK_BUTTON_BOX (bbox), 5);
  gtk_button_box_set_layout(GTK_BUTTON_BOX (bbox), GTK_BUTTONBOX_SPREAD);
  gtk_container_add (GTK_CONTAINER (bbox), ctx.buttonConnect);
  gtk_container_add (GTK_CONTAINER (bbox), ctx.buttonAdd);
  gtk_container_add (GTK_CONTAINER (bbox), ctx.buttonRemove);
  gtk_button_box_set_child_size(GTK_BUTTON_BOX(bbox), 100, 20);
  gtk_box_pack_start(GTK_BOX(vBox), bbox, FALSE, FALSE, 0);


  /* Create the list model */
  ctx.listStore = gtk_list_store_new(COLUMNS_HEADER_LENGTH, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_ULONG);
  ctx.listView = gtk_tree_view_new_with_model(GTK_TREE_MODEL(ctx.listStore));
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(ctx.listView));
  gtk_tree_selection_set_mode(selection, GTK_SELECTION_MULTIPLE);
  /* create the first column */
  cellRenderer = gtk_cell_renderer_text_new();
  for(i = 0; i < COLUMNS_HEADER_LENGTH; i++) {
    column = gtk_tree_view_column_new_with_attributes(columns_header[i], cellRenderer, "text", i, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(ctx.listView), column);
  }
 
  scrollbar = gtk_scrolled_window_new(NULL, NULL);
  /* add the list to the scrollbar and the scrollbar to the main container */
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollbar), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_container_add(GTK_CONTAINER(scrollbar),ctx.listView);
  gtk_box_pack_start(GTK_BOX(vBox), scrollbar, TRUE, TRUE, 0);

    
  /* add all the signals */
  g_signal_connect(G_OBJECT(ctx.window), "delete-event", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(G_OBJECT(ctx.statusIcon), "activate", G_CALLBACK(gtk_status_icon_activate), &ctx);
  g_signal_connect(GTK_STATUS_ICON(ctx.statusIcon), "popup-menu", GTK_SIGNAL_FUNC(gtk_status_icon_popup), &ctx);
  g_signal_connect(G_OBJECT(ctx.menuItemView), "activate", G_CALLBACK (gtk_status_icon_activate), &ctx);
  g_signal_connect(G_OBJECT(ctx.menuItemExit), "activate", G_CALLBACK (gtk_main_quit), &ctx);
  g_signal_connect(G_OBJECT(ctx.buttonConnect), "clicked", G_CALLBACK(gtk_global_button_clicked), &ctx);
  g_signal_connect(G_OBJECT(ctx.buttonAdd), "clicked", G_CALLBACK(gtk_global_button_clicked), &ctx);
  g_signal_connect(G_OBJECT(ctx.buttonRemove), "clicked", G_CALLBACK(gtk_global_button_clicked), &ctx);

  /* disable components */
  gtk_widget_set_sensitive(ctx.buttonAdd, FALSE);
  gtk_widget_set_sensitive(ctx.buttonRemove, FALSE);
  gtk_widget_set_sensitive(ctx.listView, FALSE);

  /* show the window */
  gtk_widget_show_all(ctx.window);

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
 * @fn static void gtk_status_icon_popup(GtkStatusIcon *status_icon, guint button, guint32 activate_time, gpointer p_data)
 * @brief Display the popupmenu when the user made a right click on the systray icon.
 * @param status_icon status_icon
 * @param button button
 * @param activate_time activate_time
 * @param p_data p_data
 */
static void gtk_status_icon_popup(GtkStatusIcon *status_icon, guint button, guint32 activate_time, gpointer p_data) {
  struct gtk_ctx_s *ctx = (struct gtk_ctx_s*)p_data;
  gtk_menu_popup(GTK_MENU(ctx->menu), NULL, NULL, gtk_status_icon_position_menu, status_icon, button, activate_time);
}

/**
 * @fn static void gtk_status_icon_activate (GtkStatusIcon * status_icon, gpointer p_data)
 * @brief Display or hide the main window (view menu action).
 * @param status_icon status_icon
 * @param p_data p_data
 */
static void gtk_status_icon_activate(GtkStatusIcon * status_icon, gpointer p_data) {
  struct gtk_ctx_s *ctx = (struct gtk_ctx_s*)p_data;
 
  g_return_if_fail(status_icon != NULL);
  g_return_if_fail(ctx->window != NULL);
  g_return_if_fail(GTK_IS_WIDGET (ctx->window));
 
  if (GTK_WIDGET_VISIBLE (ctx->window)) {
    gtk_widget_hide(ctx->window);
    gtk_menu_item_set_label(GTK_MENU_ITEM(ctx->menuItemView), "Show");
  } else {
    gtk_widget_show(ctx->window);
    gtk_menu_item_set_label(GTK_MENU_ITEM(ctx->menuItemView), "Hide");
  }
}

/**
 * @fn static void gtk_global_button_clicked(GtkWidget* button, gpointer p_data)
 * @brief Main callback for the buttons signal.
 * @param button Owner button.
 * @param p_data p_data
 */
static void gtk_global_button_clicked(GtkWidget* button, gpointer p_data) {
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

/**
 * @fn static void gtk_tree_view_remove_selected_items(GtkTreeView *treeview)
 * @brief Remove the selected items from the treeview (I've found this function after a research on google).
 * @param treeview The tree view.
 */
static void gtk_tree_view_remove_selected_items(GtkTreeView *treeview) {
  GtkTreeSelection *selection;
  GtkListStore *store;
  GtkTreeModel *model;
  GtkTreeIter iter;
  GList *list;
  int nRemoved, ipath;
  GString *fixed_path;
  GtkTreePath *path;
  
  selection = gtk_tree_view_get_selection (treeview);
  if(gtk_tree_selection_count_selected_rows(selection) == 0)
    return;
   
  list = gtk_tree_selection_get_selected_rows(selection, &model);
  store = GTK_LIST_STORE(model);
  
  nRemoved = 0;
  while(list) {
    ipath = atoi(gtk_tree_path_to_string(list->data));
    ipath-=nRemoved;
    fixed_path = g_string_new("");
    g_string_printf(fixed_path, "%d", ipath);
      
    path = gtk_tree_path_new_from_string(fixed_path->str);
    g_string_free(fixed_path, TRUE);
      
    if (path) {
      if (gtk_tree_model_get_iter(model, &iter, path)) { /* get iter from specified path */
	gtk_list_store_remove(store, &iter); /* remove item */
	nRemoved++;   
      }
      else { /* invalid path */
	g_error("Error!!!\n");
      }
      gtk_tree_path_free (path);
    }
    else {
      g_error("Error!!!\n");
    }
    list = list->next;
  }
  g_list_foreach(list, (GFunc)gtk_tree_path_free, NULL);
  g_list_free(list);
}


/**
 * @fn static void gtk_show_msg(struct gtk_ctx_s *ctx, int type, const char* msg)
 * @brief Show a message.
 * @param ctx The context.
 * @param type The message type (eg: GTK_MESSAGE_INFO).
 * @param msg The message to display
 */
static void gtk_show_msg(struct gtk_ctx_s *ctx, int type, const char* msg) {
  GtkWidget *b;
  b = gtk_message_dialog_new (GTK_WINDOW(ctx->window),
			      GTK_DIALOG_MODAL, type, GTK_BUTTONS_OK, "\n%s", msg);
  gtk_dialog_run(GTK_DIALOG(b));
  gtk_widget_destroy(b);
}
