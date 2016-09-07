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
#include "QManagerWindow.hpp"
#include "ui_QManagerWindow.h"
#include <QString>
#include <QStringList>
#include <QStandardItemModel>
#include <QMessageBox>
#include <QDebug>
#include <errno.h>
#include <string.h>

#define CONNECT_BUTTON_TEXT    "Connect"
#define DISCONNECT_BUTTON_TEXT "Disconnect"



QManagerWindow::QManagerWindow(QWidget *parent) :
  QDialog(parent), ui(new Ui::QManagerWindow), m_nhm(new QNHM) {
  ui->setupUi(this);
  m_addRuleDialog = new QManagerDialogAddRule(this);
  ui->addRulePushButton->setEnabled(false);
  ui->removeRulePushButton->setEnabled(false);
 
  tableModel = new QTableModel(this);
  proxyModel = new QSortFilterProxyModel(this);
  proxyModel->setSourceModel(tableModel);
  proxyModel->setDynamicSortFilter(true);

  ui->rulesTableView->setModel(proxyModel);
  ui->rulesTableView->setSortingEnabled(true);
  ui->rulesTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->rulesTableView->horizontalHeader()->setStretchLastSection(true);
  ui->rulesTableView->verticalHeader()->hide();
  ui->rulesTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->rulesTableView->setSelectionMode(QAbstractItemView::SingleSelection);
  ui->rulesTableView->setColumnHidden(7, true);

  /* add type for Thread use */
  qRegisterMetaType<QNHMRule*>("QNHMRule");
  m_worker = new QNHMWorker(m_nhm);
  m_workerThread = new QThread;
  m_worker->moveToThread(m_workerThread);

  connect(m_worker, SIGNAL(updateRule(QNHMRule*)), this, SLOT(workerUpdate(QNHMRule*)));
  connect(m_worker, SIGNAL(clearRule()), this, SLOT(workerClear()));
  connect(m_worker, SIGNAL(error(QString)), this, SLOT(workerError(QString)));
  connect(m_worker, SIGNAL(stopped()), this, SLOT(workerFinished()));
  connect(m_worker, SIGNAL(running()), this, SLOT(workerStarted()));
}

QManagerWindow::~QManagerWindow() {
  if(m_worker) {
    m_worker->stop(m_workerThread);
    delete m_worker;
    delete m_workerThread;
  }
  if(tableModel) 
    delete tableModel, tableModel = NULL;
  if(proxyModel) 
    delete proxyModel, proxyModel = NULL;
  delete m_nhm;
  delete ui;
}


/**
 * @brief Method called when the user click on the 'Add rule' button.
 */
void QManagerWindow::on_addRulePushButton_clicked() {
  QNHMRule* rule = m_addRuleDialog->display();
  if(rule) {
    qDebug() << "rule->dev: " << rule->dev;
    if(m_nhm->add(rule)) {
      QString err = "Unable to add the rule: ";
      err.append(strerror(errno));
      QMessageBox::critical(this, "NHM", err);
    }
    delete rule;
  }
}

/**
 * @brief Method called when the user click on the 'Remove rule' button.
 */
void QManagerWindow::on_removeRulePushButton_clicked() {

}

/**
 * @brief Method called when the user click on the 'Connect rule' button.
 */
void QManagerWindow::on_connectPushButton_clicked() {
  if(ui->connectPushButton->text() == DISCONNECT_BUTTON_TEXT) {
    m_worker->stop(m_workerThread);
  } else {
    /* open the connection with the module */
    if(m_nhm->open() == -1) {
      /* Display the error message */
      QString err = NHM_DEVICE_PATH;
      err.append(": ").append(strerror(errno));
      QMessageBox::critical(this, "NHM", err);
      return;
    } else {
      m_worker->start(m_workerThread);
    }
  }
}

/**
 * @brief Called when the thread has new data to display.
 * @param r The rule to display.
 */
void QManagerWindow::workerUpdate(QNHMRule* r) {
  qDebug() << "updateRule r.dev: " << r->dev;
  tableModel->insertRows(0, 1, QModelIndex());
  QStringList list = r->toStrings();
  for(int i = 0; i < list.size(); ++i) {
    QModelIndex index = tableModel->index(0, i, QModelIndex());
    tableModel->setData(index, list.at(i), Qt::DisplayRole);
  }
  list.clear();
}

/**
 * @brief Called when the thread need to clear the rules.
 */
void QManagerWindow::workerClear() {
  tableModel->clear();
}

/**
 * @brief Called when the thread is finished.
 */
void QManagerWindow::workerFinished() {
  qDebug() << "workerFinished";
  m_nhm->close();
  ui->addRulePushButton->setEnabled(false);
  ui->removeRulePushButton->setEnabled(false);
  ui->connectPushButton->setText(CONNECT_BUTTON_TEXT);
}

/**
 * @brief Called when the thread is started.
 */
void  QManagerWindow::workerStarted() {
  ui->addRulePushButton->setEnabled(true);
  ui->removeRulePushButton->setEnabled(true);
  ui->connectPushButton->setText(DISCONNECT_BUTTON_TEXT);
}


/**
 * @brief Called when an error is reached.
 */
void QManagerWindow::workerError(const QString &err) {
  QMessageBox::critical(this, "NHM", err);
}
