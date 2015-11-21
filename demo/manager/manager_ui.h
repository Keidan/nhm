/**
*******************************************************************************
* @file manager_ui.h
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
#ifndef __MANAGER_UI_H__
#define __MANAGER_UI_H__

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

/**
 * @fn void gtk_manager_build_main_ui_and_show(struct gtk_ctx_s *ctx)
 * @brief Build the main UI and show the window.
 * @param ctx The main CTX.
 */
void gtk_manager_build_main_ui_and_show(struct gtk_ctx_s *ctx);


/**
 * @fn void gtk_global_button_clicked(GtkWidget *button, gpointer p_data)
 * @brief Main callback for the buttons signal.
 * @param button Owner button.
 * @param p_data p_data
 */
void gtk_global_button_clicked(GtkWidget *button, gpointer p_data);


/**
 * @fn void gtk_show_msg(struct gtk_ctx_s *ctx, int type, const char* msg)
 * @brief Show a message.
 * @param ctx The context.
 * @param type The message type (eg: GTK_MESSAGE_INFO).
 * @param msg The message to display
 */
void gtk_show_msg(struct gtk_ctx_s *ctx, int type, const char* msg);

/**
 * @fn void gtk_tree_view_remove_selected_items(GtkTreeView *treeview)
 * @brief Remove the selected items from the treeview (I've found this function after a research on google).
 * @param treeview The tree view.
 */
void gtk_tree_view_remove_selected_items(GtkTreeView *treeview);

#endif /* __MANAGER_UI_H__ */
