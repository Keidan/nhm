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
#include "manager_ui.h"



static char* columns_header[] = {
  "Device", "Type", "Dir", "Proto", "HW", "IP", "Applied"
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
  ctx->listStore = gtk_list_store_new(COLUMNS_HEADER_LENGTH, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_ULONG);
  ctx->listView = gtk_tree_view_new_with_model(GTK_TREE_MODEL(ctx->listStore));
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(ctx->listView));
  gtk_tree_selection_set_mode(selection, GTK_SELECTION_MULTIPLE);
  /* create the first column */
  cellRenderer = gtk_cell_renderer_text_new();
  for(i = 0; i < COLUMNS_HEADER_LENGTH; i++) {
    column = gtk_tree_view_column_new_with_attributes(columns_header[i], cellRenderer, "text", i, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(ctx->listView), column);
  }
 
  scrollbar = gtk_scrolled_window_new(NULL, NULL);
  /* add the list to the scrollbar and the scrollbar to the main container */
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollbar), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_container_add(GTK_CONTAINER(scrollbar),ctx->listView);
  gtk_box_pack_start(GTK_BOX(vBox), scrollbar, TRUE, TRUE, 0);

    
  /* add all the signals */
  g_signal_connect(G_OBJECT(ctx->window), "delete-event", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(G_OBJECT(ctx->statusIcon), "activate", G_CALLBACK(gtk_status_icon_activate), &ctx);
  g_signal_connect(GTK_STATUS_ICON(ctx->statusIcon), "popup-menu", GTK_SIGNAL_FUNC(gtk_status_icon_popup), &ctx);
  g_signal_connect(G_OBJECT(ctx->menuItemView), "activate", G_CALLBACK (gtk_status_icon_activate), &ctx);
  g_signal_connect(G_OBJECT(ctx->menuItemExit), "activate", G_CALLBACK (gtk_main_quit), &ctx);
  g_signal_connect(G_OBJECT(ctx->buttonConnect), "clicked", G_CALLBACK(gtk_global_button_clicked), &ctx);
  g_signal_connect(G_OBJECT(ctx->buttonAdd), "clicked", G_CALLBACK(gtk_global_button_clicked), &ctx);
  g_signal_connect(G_OBJECT(ctx->buttonRemove), "clicked", G_CALLBACK(gtk_global_button_clicked), &ctx);

  /* disable components */
  gtk_widget_set_sensitive(ctx->buttonAdd, FALSE);
  gtk_widget_set_sensitive(ctx->buttonRemove, FALSE);
  gtk_widget_set_sensitive(ctx->listView, FALSE);

  /* show the window */
  gtk_widget_show_all(ctx->window);
}

/**
 * @fn void gtk_tree_view_remove_selected_items(GtkTreeView *treeview)
 * @brief Remove the selected items from the treeview (I've found this function after a research on google).
 * @param treeview The tree view.
 */
void gtk_tree_view_remove_selected_items(GtkTreeView *treeview) {
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
 * @fn  void gtk_show_msg(struct gtk_ctx_s *ctx, int type, const char* msg)
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
