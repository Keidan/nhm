/**
 * @file QManagerWindow.cpp
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
#include "QManagerWindow.hpp"
#include "ui_QManagerWindow.h"
#include <QString>
#include <QStringList>
#include <QStandardItemModel>
#include <QMessageBox>
#include <QDebug>
#include <errno.h>
#include <string.h>

constexpr char CONNECT_BUTTON_TEXT    [] = "Connect\0";
constexpr char DISCONNECT_BUTTON_TEXT [] = "Disconnect\0";



QManagerWindow::QManagerWindow(QWidget *parent) :
  QDialog(parent), m_ui(new Ui::QManagerWindow), m_nhm(new QNHM())
{
  m_ui->setupUi(this);
  m_addRuleDialog = new QManagerDialogAddRule(this);
  m_ui->addRulePushButton->setEnabled(false);
  m_ui->removeRulePushButton->setEnabled(false);

  m_tableModel = new QTableModel(this);
  m_proxyModel = new QSortFilterProxyModel(this);
  m_proxyModel->setSourceModel(m_tableModel);
  m_proxyModel->setDynamicSortFilter(true);

  m_ui->rulesTableView->setModel(m_proxyModel);
  m_ui->rulesTableView->setSortingEnabled(true);
  m_ui->rulesTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_ui->rulesTableView->horizontalHeader()->setStretchLastSection(true);
  m_ui->rulesTableView->verticalHeader()->hide();
  m_ui->rulesTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_ui->rulesTableView->setSelectionMode(QAbstractItemView::SingleSelection);
  m_ui->rulesTableView->setColumnHidden(COLUMN_HIDDEN, true);


  /* add type for Thread use */
  qRegisterMetaType<QNHMRule*>("QNHMRule");
  m_worker = new QNHMWorker(*m_nhm);
  m_workerThread = new QThread;
  m_worker->moveToThread(m_workerThread);

  connect(m_worker, SIGNAL(updateRule(QNHMRule*)), this, SLOT(workerUpdate(QNHMRule*)));
  connect(m_worker, SIGNAL(clearRule()), this, SLOT(workerClear()));
  connect(m_worker, SIGNAL(error(QString)), this, SLOT(workerError(QString)));
  connect(m_worker, SIGNAL(stopped()), this, SLOT(workerFinished()));
  connect(m_worker, SIGNAL(running()), this, SLOT(workerStarted()));
}

QManagerWindow::~QManagerWindow()
{
  if(m_worker)
  {
    m_worker->stop(m_workerThread);
    delete m_worker;
    delete m_workerThread;
  }
  if(m_tableModel)
    delete m_tableModel, m_tableModel = NULL;
  if(m_proxyModel)
    delete m_proxyModel, m_proxyModel = NULL;
  delete m_nhm;
  delete m_ui;
}


/**
 * @brief Method called when the user click on the 'Add rule' button.
 */
auto QManagerWindow::on_addRulePushButton_clicked() -> void
{
  auto rule = m_addRuleDialog->display();
  if(rule)
  {
    if(m_nhm->add(rule))
    {
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
auto QManagerWindow::on_removeRulePushButton_clicked() -> void
{
  auto select = m_ui->rulesTableView->selectionModel();
  if(select->hasSelection())
  {
    auto index = select->currentIndex();
    auto row = index.row();
    auto r = reinterpret_cast<QNHMRule *>(m_tableModel->getColumn(row, COLUMN_HIDDEN).value<void*>());
    m_nhm->remove(r);
    m_tableModel->removeRows(row, 1);
  }
}

/**
 * @brief Method called when the user click on the 'Connect rule' button.
 */
auto QManagerWindow::on_connectPushButton_clicked() -> void
{
  if(m_ui->connectPushButton->text() == DISCONNECT_BUTTON_TEXT)
  {
    m_worker->stop(m_workerThread);
  }
  else
  {
    /* open the connection with the module */
    if(m_nhm->open() == -1)
    {
      /* Display the error message */
      QString err = NHM_DEVICE_PATH;
      err.append(": ").append(strerror(errno));
      QMessageBox::critical(this, "NHM", err);
      return;
    }
    else
    {
      m_worker->start(m_workerThread);
    }
  }
}

/**
 * @brief Called when the thread has new data to display.
 * @param r The rule to display.
 */
auto QManagerWindow::workerUpdate(QNHMRule* r) -> void
{
  m_tableModel->insertRows(0, 1, QModelIndex());
  auto list = r->toStrings();

  for(auto i = 0; i < list.size(); ++i)
  {
    auto index = m_tableModel->index(0, i, QModelIndex());
    m_tableModel->setData(index, list.at(i), Qt::DisplayRole);
  }

  auto index = m_tableModel->index(0, COLUMN_HIDDEN, QModelIndex());
  m_tableModel->setData(index, qVariantFromValue((void *) r), Qt::DisplayRole);
  list.clear();
}

/**
 * @brief Called when the thread need to clear the rules.
 */
auto QManagerWindow::workerClear() -> void
{
  m_tableModel->clear();
}

/**
 * @brief Called when the thread is finished.
 */
auto QManagerWindow::workerFinished() -> void
{
  m_nhm->close();
  m_ui->addRulePushButton->setEnabled(false);
  m_ui->removeRulePushButton->setEnabled(false);
  m_ui->connectPushButton->setText(CONNECT_BUTTON_TEXT);
}

/**
 * @brief Called when the thread is started.
 */
auto QManagerWindow::workerStarted() -> void
{
  m_ui->addRulePushButton->setEnabled(true);
  m_ui->removeRulePushButton->setEnabled(true);
  m_ui->connectPushButton->setText(DISCONNECT_BUTTON_TEXT);
}


/**
 * @brief Called when an error is reached.
 */
auto QManagerWindow::workerError(const QString &err) -> void
{
  QMessageBox::critical(this, "NHM", err);
}
