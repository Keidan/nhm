/**
 * @file QManagerWindow.hpp
 * @author Keidan
 * @copyright GNU GENERAL PUBLIC LICENSE Version 3
 *
 *    _____
 *   /     \ _____    ____ _____     ____   ___________
 *  /  \ /  \\__  \  /    \\__  \   / ___\_/ __ \_  __  \
 * /    Y    \/ __ \|   |  \/ __ \_/ /_/  >  ___/|  | \/
 * \____|__  (____  /___|  (____  /\___  / \___  >__|
 *         \/     \/     \/     \//_____/      \/
 *
 */
#ifndef QMANAGERWINDOW_HPP
  #define QMANAGERWINDOW_HPP

  #include <QDialog>
  #include "QManagerDialogAddRule.hpp"
  #include "../model/QNHMWorker.hpp"
  #include <QThread>
  #include <QVariant>
  #include "QTableModel.hpp"
  #include <QSortFilterProxyModel>

  namespace Ui
  {
    class QManagerWindow;
  }

  class QManagerWindow : public QDialog
  {
      Q_OBJECT

    public:
      explicit QManagerWindow(QWidget *parent = 0);
      virtual ~QManagerWindow();

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

      /**
       * @brief Called when the thread has new data to display.
       * @param rule The rule to display.
       */
      void workerUpdate(QNHMRule* rule);

      /**
       * @brief Called when the thread need to clear the rules.
       */
      void workerClear();

      /**
       * @brief Called when the thread is finished.
       */
      void workerFinished();

      /**
       * @brief Called when the thread is started.
       */
      void workerStarted();

      /**
       * @brief Called when an error is reached.
       */
      void workerError(const QString &err);

    private:
      Ui::QManagerWindow *m_ui;
      QManagerDialogAddRule *m_addRuleDialog;
      QNHM *m_nhm;
      QThread *m_workerThread;
      QNHMWorker *m_worker;
      QTableModel *m_tableModel;
      QSortFilterProxyModel *m_proxyModel;

  };

#endif // QMANAGERWINDOW_HPP
