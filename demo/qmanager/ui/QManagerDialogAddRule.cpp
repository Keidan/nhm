/**
 *******************************************************************************
 * @file QManagerDialogAddRule.cpp
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
#include "QManagerDialogAddRule.hpp"
#include <QMessageBox>
#include <QIntValidator>
#include <ctype.h>
#include <limits>


#define TO_CSTR(obj) obj.toStdString().c_str()

QManagerDialogAddRule::QManagerDialogAddRule(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QManagerDialogAddRule) {

  ui->setupUi(this);
  setModal(true);

  ui->portStartLineEdit->setValidator(new QIntValidator(0, 65535, this));
  ui->portEndLineEdit->setValidator(new QIntValidator(0, 65535, this));
  int max = std::numeric_limits<int>::max();
  ui->ethProtoLineEdit->setValidator(new QIntValidator(0, max, this));
  ui->ipProtoLineEdit->setValidator(new QIntValidator(0, max, this));
}

QManagerDialogAddRule::~QManagerDialogAddRule() {
  delete ui;
}


QNHMRule* QManagerDialogAddRule::display() {
  foreach(QLineEdit *widget, this->findChildren<QLineEdit*>())
    widget->clear();
  exec();
  struct in_addr in4;
  struct in6_addr in6;
  int i;
  bool entry = false;
  QNHMRule* rule = new QNHMRule;

  /* add name */
  if(!ui->devnameLineEdit->text().isEmpty()) {
    rule->fromString(COLUMN_DEVICE, ui->devnameLineEdit->text());
    entry = true;
  }

  /* add direction */
  QString text = ui->directionCombobox->currentText();
  rule->fromString(COLUMN_DIR, text);

  /* add nf type */
  text = ui->nfTypeCombobox->currentText();
  rule->fromString(COLUMN_TYPE, text);

  /* convert ip */
  if(!ui->ipLineEdit->text().isEmpty()) {
    const char* ip = TO_CSTR(ui->ipLineEdit->text());
    if(inet_pton(AF_INET, ip, &in4) == -1) {
      if(inet_pton(AF_INET6, ip, &in6) != -1) {
        memcpy(rule->ip6, in6.s6_addr, NHM_LEN_IPv6);
        entry = true;
      } else
        QMessageBox::warning(this, "NHM", "Invalid IP address!");
    } else {
      entry = true;
      rule->ip4 = in4.s_addr;
    }
  }
  if(!ui->macLineEdit->text().isEmpty()) {
    QString s = ui->macLineEdit->text();
    if(s.size() != 17)
      QMessageBox::warning(this, "NHM", "Invalid MAC address length!");
    else {
      /* test the mac addr validity */
      for(i = 0; i != s.size(); i++)
        if(!isxdigit(s.at(i).toLatin1()) && s.at(i).toLatin1() != ':') break;
      if(i != s.size())
        QMessageBox::warning(this, "NHM", "Invalid MAC address format!");
      else {
	rule->fromString(COLUMN_HW, s);
        entry = true;
      }
    }
  }
  if(!ui->portStartLineEdit->text().isEmpty() || !ui->portEndLineEdit->text().isEmpty()) {
    QString str = ui->portStartLineEdit->text();
    if(!ui->portEndLineEdit->text().isEmpty()) {
      str.append(":").append(ui->portEndLineEdit->text());
    }
    rule->fromString(COLUMN_PORT, str);
    entry = true;
  }
  
  if(!ui->ethProtoLineEdit->text().isEmpty() || !ui->ipProtoLineEdit->text().isEmpty()) {
    QString str = ui->ethProtoLineEdit->text();
    if(!ui->ipProtoLineEdit->text().isEmpty()) {
      str.append(" - ").append(ui->ipProtoLineEdit->text());
    }
    rule->fromString(COLUMN_PROTO, str);
    entry = true;
  }
  if(!entry) {
    delete rule, rule = NULL;
  }

  return rule;
}
