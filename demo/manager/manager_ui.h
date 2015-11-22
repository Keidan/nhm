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
#include <nhm.h>


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
 * @fn unsigned int gtk_tree_view_get_headers_length(void)
 * @brief Get the number of the headers.
 * @return The length.
 */
unsigned int gtk_tree_view_get_headers_length(void);


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
 * @fn void gtk_tree_view_remove_selected_items(struct gtk_ctx_s *ctx, void (*user_on_remove)(struct gtk_ctx_s *ctx, GtkTreeModel *model, GtkTreeIter *iter))
 * @brief Remove the selected items from the treeview (I've found this function after a research on google).
 * @param user_on_remove On remove fuction.
 */
void gtk_tree_view_remove_selected_items(struct gtk_ctx_s *ctx, void (*user_on_remove)(struct gtk_ctx_s *ctx, GtkTreeModel *model, GtkTreeIter *iter));

/**
 * @fn void gtk_list_view_clear_all(struct gtk_ctx_s *ctx, void (*user_on_remove)(struct gtk_ctx_s *ctx, GtkTreeModel *model, GtkTreeIter *iter))
 * @brief Remove all elements from the list view.
 * @param ctx The globale CTX.
 * @param user_on_remove User function to release the resource.
 */
void gtk_list_view_clear_all(struct gtk_ctx_s *ctx, void (*user_on_remove)(struct gtk_ctx_s *ctx, GtkTreeModel *model, GtkTreeIter *iter));

/**
 * @fn void gtk_tree_view_add_row(struct gtk_ctx_s *ctx, struct nhm_s *rule)
 * @breif Add a new row.
 * @param ctx The globale CTX.
 * @param rule The rule to add.
 */
void gtk_tree_view_add_row(struct gtk_ctx_s *ctx, struct nhm_s *rule);

/**
 * @fn gboolean gtk_show_add_dialog(struct gtk_ctx_s *ctx, struct nhm_s *rule)
 * @brief Show the 'ad' dialog.
 * @param ctx The globale CTX.
 * @param rule The rule output.
 * @return TRUE if the event is consumed.
 */
gboolean gtk_show_add_dialog(struct gtk_ctx_s *ctx, struct nhm_s *rule);

#endif /* __MANAGER_UI_H__ */
