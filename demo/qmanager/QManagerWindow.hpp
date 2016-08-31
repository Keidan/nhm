/**
 *******************************************************************************
 * @file QManagerWindow.hpp
 * @author Keidan
 * @date 30/08/2016
 * @par Project nhm->qmanager
 *
 * @par Copyright 2016 Keidan, all right reserved
 *
 *      This software is distributed in the hope that it will be useful, but
 *      WITHOUT ANY WARRANTY.
 *
 *      License summary : You can modify and redistribute the sources code and
 *      binaries. You can send me the bug-fix
 *
 *      Term of the license in in the file license.txt.
 *    _____
 *   /     \ _____    ____ _____     ____   ___________
 *  /  \ /  \\__  \  /    \\__  \   / ___\_/ __ \_  __  \
 * /    Y    \/ __ \|   |  \/ __ \_/ /_/  >  ___/|  | \/
 * \____|__  (____  /___|  (____  /\___  / \___  >__|
 *         \/     \/     \/     \//_____/      \/
 *******************************************************************************
 */
#ifndef QMANAGERWINDOW_HPP
  #define QMANAGERWINDOW_HPP

  #include <QDialog>
  #include "QManagerDialogAddRule.hpp"
  #include "QThreadTableView.hpp"

  namespace Ui {
    class QManagerWindow;
  }

  class QManagerWindow : public QDialog {
    Q_OBJECT

    public:
      explicit QManagerWindow(QWidget *parent = 0);
      ~QManagerWindow();

    private slots:

      /**
       * @brief Method called when the user click on the 'Add rule' button.
       */
      void on_addRulePushButton_clicked();

      /**
       * @brief Method called when the user click on the 'Remove rule' button.
       */
      void on_removeRulePushButton_clicked();

      /**
       * @brief Method called when the user click on the 'Connect rule' button.
       */
      void on_connectPushButton_clicked();

      void showNewRules(const struct nhm_s &data);

    private:
      Ui::QManagerWindow *ui;
      QManagerDialogAddRule *m_addRuleDialog;
      QNHM *m_nhm;
      QThreadTableView *m_thread;
  };

#endif // QMANAGERWINDOW_HPP
