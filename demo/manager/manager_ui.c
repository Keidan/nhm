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
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include "manager_ui.h"

#include <netinet/in.h>
#include <arpa/inet.h>


static char* columns_header[] = {
  "Device", "Type", "Dir", "Proto", "HW", "IP", "Applied", "Hide"
};

#define COLUMNS_HEADER_LENGTH (sizeof(columns_header) / sizeof(columns_header[0]))


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
 * @fn void gtk_manager_build_main_ui_and_show(struct gtk_ctx_s *ctx)
 * @brief Build the main UI and show the window.
 * @param ctx The main CTX.
 */
void gtk_manager_build_main_ui_and_show(struct gtk_ctx_s *ctx) {
  GtkCellRenderer *cellRenderer;
  GtkTreeViewColumn* column;
  GtkWidget *scrollbar, *vBox;
  unsigned int i;
  GtkWidget* bbox;
  GtkTreeSelection *selection;

  /* Create the main window */
  ctx->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(ctx->window), "NHM Manager");
  gtk_window_set_position(GTK_WINDOW(ctx->window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(ctx->window), 500, 300);

  /* create the main container */
  vBox = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(ctx->window), vBox);

  /* Add to the systray */
  ctx->statusIcon = gtk_status_icon_new_from_stock (GTK_STOCK_NETWORK);
  gtk_status_icon_set_tooltip (GTK_STATUS_ICON (ctx->statusIcon), "NHM Manager");

  /* Create the popupmenu used by the systray. */
  ctx->menu = gtk_menu_new();
  ctx->menuItemView = gtk_menu_item_new_with_label ("Hide");
  ctx->menuItemExit = gtk_menu_item_new_with_label ("Exit");
  ctx->menuItemSeparator = gtk_separator_menu_item_new();
 
  /* add the submenu to the popupmenu */
  gtk_menu_shell_append (GTK_MENU_SHELL (ctx->menu), ctx->menuItemView);
  gtk_menu_shell_append (GTK_MENU_SHELL (ctx->menu), ctx->menuItemSeparator);
  gtk_menu_shell_append (GTK_MENU_SHELL (ctx->menu), ctx->menuItemExit);
  gtk_widget_show_all (ctx->menu);

  /* Create the buttons */
  ctx->buttonConnect = gtk_toggle_button_new_with_label("Connect");
  ctx->buttonAdd = gtk_button_new_with_label("Add");
  ctx->buttonRemove = gtk_button_new_with_label("Remove");
  bbox = gtk_hbutton_box_new();
  gtk_container_set_border_width (GTK_CONTAINER (bbox), 5);
  gtk_button_box_set_spacing (GTK_BUTTON_BOX (bbox), 5);
  gtk_button_box_set_layout(GTK_BUTTON_BOX (bbox), GTK_BUTTONBOX_SPREAD);
  gtk_container_add (GTK_CONTAINER (bbox), ctx->buttonConnect);
  gtk_container_add (GTK_CONTAINER (bbox), ctx->buttonAdd);
  gtk_container_add (GTK_CONTAINER (bbox), ctx->buttonRemove);
  gtk_button_box_set_child_size(GTK_BUTTON_BOX(bbox), 100, 20);
  gtk_box_pack_start(GTK_BOX(vBox), bbox, FALSE, FALSE, 0);

  /* Create the list model */
  ctx->listStore = gtk_list_store_new(COLUMNS_HEADER_LENGTH, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_ULONG, G_TYPE_POINTER);
  ctx->listView = gtk_tree_view_new_with_model(GTK_TREE_MODEL(ctx->listStore));
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(ctx->listView));
  gtk_tree_selection_set_mode(selection, GTK_SELECTION_MULTIPLE);
  /* create the first column */
  cellRenderer = gtk_cell_renderer_text_new();
  for(i = 0; i < COLUMNS_HEADER_LENGTH; i++) {
    if(i == COLUMNS_HEADER_LENGTH - 1)
      column = gtk_tree_view_column_new();
    else
      column = gtk_tree_view_column_new_with_attributes(columns_header[i], cellRenderer, "text", i, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(ctx->listView), column);
  }
  /* hide the latest column. */
  gtk_tree_view_column_set_visible(column, FALSE);

  scrollbar = gtk_scrolled_window_new(NULL, NULL);
  /* add the list to the scrollbar and the scrollbar to the main container */
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollbar), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_container_add(GTK_CONTAINER(scrollbar),ctx->listView);
  gtk_box_pack_start(GTK_BOX(vBox), scrollbar, TRUE, TRUE, 0);

    
  /* add all the signals */
  g_signal_connect(G_OBJECT(ctx->window), "delete-event", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(G_OBJECT(ctx->statusIcon), "activate", G_CALLBACK(gtk_status_icon_activate), ctx);
  g_signal_connect(GTK_STATUS_ICON(ctx->statusIcon), "popup-menu", GTK_SIGNAL_FUNC(gtk_status_icon_popup), ctx);
  g_signal_connect(G_OBJECT(ctx->menuItemView), "activate", G_CALLBACK (gtk_status_icon_activate), ctx);
  g_signal_connect(G_OBJECT(ctx->menuItemExit), "activate", G_CALLBACK (gtk_main_quit), ctx);
  g_signal_connect(G_OBJECT(ctx->buttonConnect), "clicked", G_CALLBACK(gtk_global_button_clicked), ctx);
  g_signal_connect(G_OBJECT(ctx->buttonAdd), "clicked", G_CALLBACK(gtk_global_button_clicked), ctx);
  g_signal_connect(G_OBJECT(ctx->buttonRemove), "clicked", G_CALLBACK(gtk_global_button_clicked), ctx);

  /* disable components */
  gtk_widget_set_sensitive(ctx->buttonAdd, FALSE);
  gtk_widget_set_sensitive(ctx->buttonRemove, FALSE);
  gtk_widget_set_sensitive(ctx->listView, FALSE);

  /* show the window */
  gtk_widget_show_all(ctx->window);
}

/**
 * @fn unsigned int gtk_tree_view_get_headers_length(void)
 * @brief Get the number of the headers.
 * @return The length.
 */
unsigned int gtk_tree_view_get_headers_length(void) {
  return COLUMNS_HEADER_LENGTH;
}

/**
 * @fn void gtk_tree_view_remove_selected_items(struct gtk_ctx_s *ctx, void (*user_on_remove)(struct gtk_ctx_s *ctx, GtkTreeModel *model, GtkTreeIter *iter))
 * @brief Remove the selected items from the treeview (I've found this function after a research on google).
 * @param user_on_remove On remove fuction.
 */
void gtk_tree_view_remove_selected_items(struct gtk_ctx_s *ctx, void (*user_on_remove)(struct gtk_ctx_s *ctx, GtkTreeModel *model, GtkTreeIter *iter)) {
  GtkTreeSelection *selection;
  GtkListStore *store;
  GtkTreeModel *model;
  GtkTreeIter iter;
  GList *list;
  int nRemoved, ipath;
  GString *fixed_path;
  GtkTreePath *path;
  GtkTreeView *treeview = GTK_TREE_VIEW(ctx->listView);
  
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
	if(user_on_remove) user_on_remove(ctx, model, &iter);
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
 * @fn static gboolean gtk_foreach_rem(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, GList **rowref_list)
 * @brief Callback function for gtk_list_view_clear_all
 * @apram model GtkTreeModel*
 * @param path  GtkTreePath*
 * @param iter GtkTreeIter*
 * @param rowref_list GList**
 */
static gboolean gtk_foreach_rem(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, GList **rowref_list) {
  GtkTreeRowReference  *rowref;
  g_assert ( iter != NULL );
  rowref = gtk_tree_row_reference_new(model, path);
  *rowref_list = g_list_append(*rowref_list, rowref);
  return FALSE; /* do not stop walking the store, call us with next row */
}

/**
 * @fn void gtk_list_view_clear_all(struct gtk_ctx_s *ctx, void (*user_on_remove)(struct gtk_ctx_s *ctx, GtkTreeModel *model, GtkTreeIter *iter))
 * @brief Remove all elements from the list view.
 * @param ctx The globale CTX.
 * @param user_on_remove User function to release the resource.
 */
void gtk_list_view_clear_all(struct gtk_ctx_s *ctx, void (*user_on_remove)(struct gtk_ctx_s *ctx, GtkTreeModel *model, GtkTreeIter *iter)) {
  GList *rr_list = NULL;    /* list of GtkTreeRowReferences to remove */
  GList *node;
  GtkListStore* store = ctx->listStore;
  gtk_tree_model_foreach(GTK_TREE_MODEL(store), (GtkTreeModelForeachFunc) gtk_foreach_rem, &rr_list);

  for(node = rr_list; node != NULL; node = node->next) {
    GtkTreePath *path;
    path = gtk_tree_row_reference_get_path((GtkTreeRowReference*)node->data);
    if (path) {
      GtkTreeIter  iter;
      if (gtk_tree_model_get_iter(GTK_TREE_MODEL(store), &iter, path)) {
	if(user_on_remove) user_on_remove(ctx, GTK_TREE_MODEL(store), &iter);
	gtk_list_store_remove(store, &iter);
      }
    }
  }
  g_list_foreach(rr_list, (GFunc) gtk_tree_row_reference_free, NULL);
  g_list_free(rr_list);
}


/**
 * @fn void gtk_show_msg(struct gtk_ctx_s *ctx, int type, const char* msg)
 * @brief Show a message.
 * @param ctx The context.
 * @param type The message type (eg: GTK_MESSAGE_INFO).
 * @param msg The message to display
 */
void gtk_show_msg(struct gtk_ctx_s *ctx, int type, const char* msg) {
  GtkWidget *b;
  b = gtk_message_dialog_new (GTK_WINDOW(ctx->window),
			      GTK_DIALOG_MODAL, type, GTK_BUTTONS_OK, "\n%s", msg);
  gtk_dialog_run(GTK_DIALOG(b));
  gtk_widget_destroy(b);
}

/**
 * @fn void gtk_tree_view_add_row(struct gtk_ctx_s *ctx, struct nhm_s *rule)
 * @breif Add a new row.
 * @param ctx The globale CTX.
 * @param rule The rule to add.
 */
void gtk_tree_view_add_row(struct gtk_ctx_s *ctx, struct nhm_s *rule) {
  GString *str_proto, *str_hw, *str_ip, *str_type, *str_applied;
  unsigned char buffer [4];
  GtkTreeIter iter;
  time_t nowtime;
  struct tm *nowtm;
  char tmbuf[64], buf[64];
 
	
  str_proto = g_string_sized_new(18);
  str_hw = g_string_sized_new(18);
  str_ip = g_string_sized_new(30);
  str_type = g_string_sized_new(30);
  str_applied = g_string_sized_new(50);
  g_string_append_printf(str_hw, "%02x:%02x:%02x:%02x:%02x:%02x", rule->hw[0], rule->hw[1], rule->hw[2], rule->hw[3], rule->hw[4], rule->hw[5]);
  if(rule->ip4) {
    nhm_from_ipv4(buffer, 0, rule->ip4);
    g_string_append_printf(str_ip, "%d.%d.%d.%d", buffer[0], buffer[1], buffer[2], buffer[3]);
    if(!rule->port[1])
      g_string_append_printf(str_ip, ":%d", rule->port[0]);
    else
      g_string_append_printf(str_ip, ":[%d-%d]", rule->port[0], rule->port[1]);
  }
  if(!str_ip->str[0])
    g_string_append(str_ip, "unknown");

  if(rule->applied.last.tv_sec) {
    nowtime = rule->applied.last.tv_sec;
    nowtm = localtime(&nowtime);
    strftime(tmbuf, sizeof(tmbuf), "%Y-%m-%d %H:%M:%S", nowtm);
    snprintf(buf, sizeof(buf), "%s.%06ld", tmbuf, (rule->applied.last.tv_nsec / 1000));
    g_string_append_printf(str_applied, "%ld (%s)", rule->applied.counter, buf);
  } else
    g_string_append_printf(str_applied, "%ld (never)", rule->applied.counter);
  g_string_append_printf(str_proto, "%d-%d", rule->eth_proto, rule->ip_proto);


  gtk_list_store_append(ctx->listStore, &iter);
  gtk_list_store_set(ctx->listStore, &iter, 
		     0, rule->dev[0] ? "All" : rule->dev, 
		     1, str_type->str,
		     2, (rule->dir == NHM_DIR_INPUT ? "input" : (rule->dir == NHM_DIR_OUTPUT ? "output" : "both")), 
		     3, str_proto->str,  
		     4, str_hw->str, 
		     5, str_ip->str, 
		     6, str_applied,
		     7, rule,
		     -1);
  g_string_free(str_proto, TRUE);
  g_string_free(str_hw, TRUE);
  g_string_free(str_ip, TRUE);
  g_string_free(str_type, TRUE);

}

/**
 * @fn static int str2int(char* str, int def)
 * @brief Convert a string to an int valude.
 * @param str The string to convert.
 * @param def The default value if fail.
 * @return The int value.
 */
static int str2int(char* str, int def) {
  int n;
  if(!str) return def;
  n = strtol(str, NULL, 10);
  if((errno == ERANGE) || (errno == EINVAL)) {
    return def;
  }
  return n;
}

/**
 * @fn gboolean gtk_show_add_dialog(struct gtk_ctx_s *ctx, struct nhm_s *rule)
 * @brief Show the 'ad' dialog.
 * @param ctx The globale CTX.
 * @param rule The rule output.
 * @return TRUE if the event is consumed.
 */
gboolean gtk_show_add_dialog(struct gtk_ctx_s *ctx, struct nhm_s *rule) {
  gboolean res = FALSE, entry = FALSE;
  GtkWidget* gw_box;
  GtkWidget* gw_vBox;
  GtkWidget* gw_dev;
  GtkWidget* gw_nfType;
  GtkWidget* gw_dir;
  GtkWidget* gw_hw;
  GtkWidget* gw_ip;
  GtkWidget* gw_port1;
  GtkWidget* gw_port2;
  GtkWidget* gw_ethProto;
  GtkWidget* gw_ipProto;
  GtkWidget* gw_table;
  gchar* gc_dev;
  gchar* gc_nfType;
  gchar* gc_dir;
  gchar* gc_hw;
  gchar* gc_ip;
  gchar* gc_port1;
  gchar* gc_port2;
  gchar* gc_ethProto;
  gchar* gc_ipProto;
  struct in_addr in4;
  struct in6_addr in6;
  unsigned int i, length;
  unsigned int imac[6];

  gw_box = gtk_dialog_new_with_buttons("Add a new rule",
				       GTK_WINDOW(ctx->window),
				       GTK_DIALOG_MODAL,
				       GTK_STOCK_OK, "Ok",
				       GTK_STOCK_CANCEL, "Cancel",
				       NULL);
  gw_vBox = gtk_vbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(gw_box)->vbox), gw_vBox, TRUE, TRUE, 0);

  gw_dev = gtk_entry_new();
  gw_hw = gtk_entry_new();
  gw_ip = gtk_entry_new();
  gw_port1 = gtk_entry_new();
  gw_port2 = gtk_entry_new();
  gw_nfType = gtk_combo_box_new_text();
  gtk_combo_box_append_text(GTK_COMBO_BOX(gw_nfType), "ACCEPT");
  gtk_combo_box_append_text(GTK_COMBO_BOX(gw_nfType), "DROP");
  gtk_combo_box_append_text(GTK_COMBO_BOX(gw_nfType), "QUEUE");
  gtk_combo_box_append_text(GTK_COMBO_BOX(gw_nfType), "REPEAT");
  gtk_combo_box_append_text(GTK_COMBO_BOX(gw_nfType), "STOLEN");
  gtk_combo_box_append_text(GTK_COMBO_BOX(gw_nfType), "STOP");
  gtk_combo_box_set_active(GTK_COMBO_BOX(gw_nfType), 0);
  gw_dir = gtk_combo_box_new_text();
  gtk_combo_box_append_text(GTK_COMBO_BOX(gw_dir), "BOTH");
  gtk_combo_box_append_text(GTK_COMBO_BOX(gw_dir), "INPUT");
  gtk_combo_box_append_text(GTK_COMBO_BOX(gw_dir), "OUTPUT");
  gtk_combo_box_set_active(GTK_COMBO_BOX(gw_dir), 0);
  gw_ethProto = gtk_entry_new();
  gw_ipProto = gtk_entry_new();


  /* rows x columns */
  gw_table = gtk_table_new(6, 4, FALSE);
  gtk_box_pack_start(GTK_BOX(gw_vBox), gw_table, TRUE, TRUE, 0);
  /* ligne 1 */
  gtk_table_attach(GTK_TABLE(gw_table), gtk_label_new("Device name: "),
		   0, 1, 0, 1, GTK_SHRINK, GTK_SHRINK, 5, 5);
  gtk_table_attach(GTK_TABLE(gw_table), gw_dev,
		   1, 4, 0, 1, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), GTK_SHRINK, 5, 5);
  /* ligne 2 */
  gtk_table_attach(GTK_TABLE(gw_table), gtk_label_new("MAC: "),
		   0, 1, 1, 2, GTK_SHRINK, GTK_SHRINK, 5, 5);
  gtk_table_attach(GTK_TABLE(gw_table), gw_hw,
		   1, 4, 1, 2, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), GTK_SHRINK, 5, 5);
  /* ligne 3 */
  gtk_table_attach(GTK_TABLE(gw_table), gtk_label_new("IP: "),
		   0, 1, 2, 3, GTK_SHRINK, GTK_SHRINK, 5, 0);
  gtk_table_attach(GTK_TABLE(gw_table), gw_ip,
		   1, 4, 2, 3, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), GTK_SHRINK, 5, 5);
  /* ligne 4 */
  gtk_table_attach(GTK_TABLE(gw_table), gtk_label_new("Port start: "),
		   0, 1, 3, 4, GTK_SHRINK, GTK_SHRINK, 5, 0);
  gtk_table_attach(GTK_TABLE(gw_table), gw_port1,
		   1, 2, 3, 4, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), GTK_SHRINK, 5, 5);
  gtk_table_attach(GTK_TABLE(gw_table), gtk_label_new("Port end: "),
		   2, 3, 3, 4, GTK_SHRINK, GTK_SHRINK, 5, 0);
  gtk_table_attach(GTK_TABLE(gw_table), gw_port2,
		   3, 4, 3, 4, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), GTK_SHRINK, 5, 5);
  /* ligne 5 */
  gtk_table_attach(GTK_TABLE(gw_table), gtk_label_new("Eth proto: "),
		   0, 1, 4, 5, GTK_SHRINK, GTK_SHRINK, 5, 5);
  gtk_table_attach(GTK_TABLE(gw_table), gw_ethProto,
		   1, 2, 4, 5, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), GTK_SHRINK, 5, 5);
  gtk_table_attach(GTK_TABLE(gw_table), gtk_label_new("IP Proto: "),
		   2, 3, 4, 5, GTK_SHRINK, GTK_SHRINK, 5, 5);
  gtk_table_attach(GTK_TABLE(gw_table), gw_ipProto,
		   3, 4, 4, 5, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), GTK_SHRINK, 5, 5);
  /* ligne 6 */
  gtk_table_attach(GTK_TABLE(gw_table), gtk_label_new("Direction: "),
		   0, 1, 5, 6, GTK_SHRINK, GTK_SHRINK, 5, 5);
  gtk_table_attach(GTK_TABLE(gw_table), gw_dir,
		   1, 2, 5, 6, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), GTK_SHRINK, 5, 5);
  gtk_table_attach(GTK_TABLE(gw_table), gtk_label_new("NF Type: "),
		   2, 3, 5, 6, GTK_SHRINK, GTK_SHRINK, 5, 5);
  gtk_table_attach(GTK_TABLE(gw_table), gw_nfType,
		   3, 4, 5, 6, (GtkAttachOptions)(GTK_EXPAND | GTK_FILL), GTK_SHRINK, 5, 5);
 
  /* Affichage des éléments de la boite de dialogue */
  gtk_widget_show_all(GTK_DIALOG(gw_box)->vbox);
  gtk_window_set_resizable(GTK_WINDOW(gw_box), FALSE);

  switch (gtk_dialog_run(GTK_DIALOG(gw_box))) {
    case GTK_RESPONSE_OK:
      nhm_init_rule(rule);
      /* convert dev name */
      gc_dev = (gchar*)gtk_entry_get_text(GTK_ENTRY(gw_dev));
      if(strlen(gc_dev)) {
	strncpy(rule->dev, gc_dev, IFNAMSIZ);
	entry = TRUE;
      }
      /* convert ip */
      gc_ip = (gchar*)gtk_entry_get_text(GTK_ENTRY(gw_ip));
      if(strlen(gc_ip)) {
	if(inet_pton(AF_INET, gc_ip, &in4) == -1) {
	  if(inet_pton(AF_INET6, gc_ip, &in6) != -1) {
	    memcpy(rule->ip6, in6.s6_addr, NHM_LEN_IPv6);
	    entry = TRUE;
	  } else
	    gtk_show_msg(ctx, GTK_MESSAGE_ERROR, "Invalid IP address!");
	} else
	  entry = TRUE;
      }
      /* convert hw */
      gc_hw = (gchar*)gtk_entry_get_text(GTK_ENTRY(gw_hw));
      length = strlen(gc_hw);
      if(length) {
	if(length != 17) {
	  gtk_show_msg(ctx, GTK_MESSAGE_ERROR, "Invalid MAC address length!");
	} else {
	  for(i = 0; i != length; i++) {
	    if(!isxdigit(gc_hw[i]) && gc_hw[i] != ':') break;
	  }
	  if(i != length) 
	    gtk_show_msg(ctx, GTK_MESSAGE_ERROR, "Invalid MAC address format!");
	  else {
	    sscanf(gc_hw, "%x:%x:%x:%x:%x:%x", &imac[0], &imac[1], &imac[2], &imac[3], &imac[4], &imac[5]);
	    for(i = 0; i < 6; i++) rule->hw[i] = (unsigned char)imac[i];
	    entry = TRUE;
	  }
	}
      }
      /* convert port1 */
      gc_port1 = (gchar*)gtk_entry_get_text(GTK_ENTRY(gw_port1));
      if(strlen(gc_port1)) {
	length = (unsigned int)str2int(gc_port1, 0);
	if(length < 1 || length > 65535)
	  gtk_show_msg(ctx, GTK_MESSAGE_ERROR, "Invalid port start value!");
	else {
	  rule->port[0] = (unsigned short)length;
	  entry = TRUE;
	}
      }
      /* convert port2 */
      gc_port2 = (gchar*)gtk_entry_get_text(GTK_ENTRY(gw_port2));
      if(strlen(gc_port2)) {
	length = (unsigned int)str2int(gc_port2, 0);
	if(length < 1 || length > 65535)
	  gtk_show_msg(ctx, GTK_MESSAGE_ERROR, "Invalid port end value!");
	else {
	  rule->port[1] = (unsigned short)length;
	  entry = TRUE;
	}
      }
      /* convert eth proto */
      gc_ethProto = (gchar*)gtk_entry_get_text(GTK_ENTRY(gw_ethProto));
      if(strlen(gc_ethProto)) {
	rule->eth_proto = (unsigned short)str2int(gc_ethProto, 0);
	entry = TRUE;
      }
      /* convert ip proto */
      gc_ipProto = (gchar*)gtk_entry_get_text(GTK_ENTRY(gw_ipProto));
      if(strlen(gc_ipProto)) {
	rule->ip_proto = (unsigned short)str2int(gc_ipProto, 0);
	entry = TRUE;
      }
      /* convert dir */
      gc_dir = gtk_combo_box_get_active_text(GTK_COMBO_BOX(gw_dir));
      if(strlen(gc_dir)) {
	if(strcmp(gc_dir, "INPUT") == 0) rule->dir = NHM_DIR_INPUT;
	else if(strcmp(gc_dir, "OUTPUT") == 0) rule->dir = NHM_DIR_OUTPUT;
	else rule->dir = NHM_DIR_BOTH;
	entry = TRUE;
      }
      g_free(gc_dir);
      /* convert dir */
      gc_nfType = gtk_combo_box_get_active_text(GTK_COMBO_BOX(gw_nfType));
      if(strlen(gc_nfType)) {
	if(strcmp(gc_nfType, "DROP") == 0) rule->nf_type = NHM_NF_TYPE_DROP;
	else if(strcmp(gc_nfType, "QUEUE") == 0) rule->nf_type = NHM_NF_TYPE_QUEUE;
	else if(strcmp(gc_nfType, "REPEAT") == 0) rule->nf_type = NHM_NF_TYPE_REPEAT;
	else if(strcmp(gc_nfType, "STOLEN") == 0) rule->nf_type = NHM_NF_TYPE_STOLEN;
	else if(strcmp(gc_nfType, "STOP") == 0) rule->nf_type = NHM_NF_TYPE_STOP;
	else rule->nf_type = NHM_NF_TYPE_ACCEPT;
	entry = TRUE;
      }
      g_free(gc_nfType);
      res = entry;
      break;
    case GTK_RESPONSE_CANCEL:
    case GTK_RESPONSE_NONE:
    default:
      res = FALSE;
      break;
  }
  /* release the box. */
  gtk_widget_destroy(gw_box);
  return res;
}

