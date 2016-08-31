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
  QDialog(parent), ui(new Ui::QManagerWindow), m_nhm(new QNHM), m_thread(new QThreadTableView) {
  ui->setupUi(this);
  m_addRuleDialog = new QManagerDialogAddRule(this);
  ui->addRulePushButton->setEnabled(false);
  ui->removeRulePushButton->setEnabled(false);

  QStringList headers;
  headers << "Device" << "Type" << "Dir" << "Proto" << "HW" << "IP" << "Applied" << "Hide";
  QStandardItemModel *model = new QStandardItemModel;
  model->setHorizontalHeaderLabels(headers); 
  ui->rulesTableView->setModel(model);
  ui->rulesTableView->setColumnHidden(7, true);

  connect(m_thread, SIGNAL(newRule(struct nhm_s)), this, SLOT(showNewRules(struct nhm_s)));
}

QManagerWindow::~QManagerWindow() {
  delete m_nhm;
  delete m_thread;
  delete ui;
}

/**
 * @brief Method called when the user click on the 'Add rule' button.
 */
void QManagerWindow::on_addRulePushButton_clicked() {
  struct nhm_s* rule = m_addRuleDialog->display();
  if(rule) {
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
    m_thread->stop();
    m_nhm->close();
    ui->addRulePushButton->setEnabled(false);
    ui->removeRulePushButton->setEnabled(false);
    ui->connectPushButton->setText(CONNECT_BUTTON_TEXT);
  } else {
    /* open the connection with the module */
    if(m_nhm->open() == -1) {
      /* Display the error message */
      QString err = NHM_DEVICE_PATH;
      err.append(": ").append(strerror(errno));
      QMessageBox::critical(this, "NHM", err);
      return;
    } else {
      ui->addRulePushButton->setEnabled(true);
      ui->removeRulePushButton->setEnabled(true);
      ui->connectPushButton->setText(DISCONNECT_BUTTON_TEXT);
      m_thread->start();
    }
  }
}

void QManagerWindow::showNewRules(const struct nhm_s &data) {
  qDebug() << "showNewRules";
}
