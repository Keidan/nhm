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
#include <gtk/gtk.h>

struct gtk_ctx_s {
  GtkWidget* window;
  GtkStatusIcon *statusIcon;
    GtkWidget* menu;
    GtkWidget* menuItemView;
    GtkWidget* menuItemExit;
    GtkWidget* menuItemSeparator;
};



/**
 * @fn static void gtk_status_icon_activate (GtkStatusIcon * status_icon, gpointer p_data)
 * @brief Display or hide the main window (view menu action).
 * @param status_icon status_icon
 * @param p_data p_data
 */
static void gtk_status_icon_activate (GtkStatusIcon * pStatusIcon, gpointer p_data);

/**
 * @fn static void gtk_status_icon_popup(GtkStatusIcon *status_icon, guint button, guint32 activate_time, gpointer p_data)
 * @brief Display the popupmenu when the user made a right click on the systray icon.
 * @param status_icon status_icon
 * @param button button
 * @param activate_time activate_time
 * @param p_data p_data
 */
static void gtk_status_icon_popup(GtkStatusIcon *status_icon, guint button, guint32 activate_time, gpointer p_data);




 
int main(int argc, char** argv){

  struct gtk_ctx_s ctx;
  memset(&ctx, 0, sizeof(struct gtk_ctx_s));
  /* initialize the threads stack. */
  g_thread_init(NULL);
  /* initialize GDK. */
  gdk_threads_init();
  /* initialize GTK. */
  gtk_init(&argc, &argv);

  /* Create the main window */
  ctx.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(ctx.window), "NHM Manager");
  gtk_window_set_position(GTK_WINDOW(ctx.window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(ctx.window), 500, 300);

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

  /* add all the signals */
  g_signal_connect(G_OBJECT(ctx.window), "delete-event", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect (G_OBJECT(ctx.statusIcon), "activate", G_CALLBACK(gtk_status_icon_activate), &ctx);
  g_signal_connect(GTK_STATUS_ICON(ctx.statusIcon), "popup-menu", GTK_SIGNAL_FUNC(gtk_status_icon_popup), &ctx);
  g_signal_connect (G_OBJECT(ctx.menuItemView), "activate", G_CALLBACK (gtk_status_icon_activate), &ctx);
  g_signal_connect (G_OBJECT(ctx.menuItemExit), "activate", G_CALLBACK (gtk_main_quit), &ctx);

  /* show the window */
  gtk_widget_show_all(ctx.window);

  /* gtk main loop */
  gdk_threads_enter();
  gtk_main();
  gdk_threads_leave();

  return EXIT_SUCCESS;
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
static void gtk_status_icon_activate (GtkStatusIcon * status_icon, gpointer p_data) {
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
