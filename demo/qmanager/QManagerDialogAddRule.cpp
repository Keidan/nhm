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


struct nhm_s* QManagerDialogAddRule::display() {
  exec();
  struct in_addr in4;
  struct in6_addr in6;
  int i;
  unsigned int imac[6];
  bool entry = false;
  struct nhm_s* rule = new struct nhm_s;

  nhm_init_rule(rule);
  /* add name */
  if(!ui->devnameLineEdit->text().isEmpty()) {
    strncpy(rule->dev, TO_CSTR(ui->devnameLineEdit->text()), IFNAMSIZ);
    entry = true;
  }

  /* add direction */
  if(ui->directionCombobox->currentText() == "INPUT") rule->dir = NHM_DIR_INPUT;
  else if(ui->directionCombobox->currentText() == "OUTPUT") rule->dir = NHM_DIR_OUTPUT;
  else rule->dir = NHM_DIR_BOTH;

  /* add nf type */
  if(ui->nfTypeCombobox->currentText() == "DROP") rule->nf_type = NHM_NF_TYPE_DROP;
  else if(ui->nfTypeCombobox->currentText() == "QUEUE") rule->nf_type = NHM_NF_TYPE_QUEUE;
  else if(ui->nfTypeCombobox->currentText() == "REPEAT") rule->nf_type = NHM_NF_TYPE_REPEAT;
  else if(ui->nfTypeCombobox->currentText() == "STOLEN") rule->nf_type = NHM_NF_TYPE_STOLEN;
  else if(ui->nfTypeCombobox->currentText() == "STOP") rule->nf_type = NHM_NF_TYPE_STOP;
  else rule->nf_type = NHM_NF_TYPE_ACCEPT;

  /* convert ip */
  if(!ui->ipLineEdit->text().isEmpty()) {
    const char* ip = TO_CSTR(ui->ipLineEdit->text());
    if(inet_pton(AF_INET, ip, &in4) == -1) {
      if(inet_pton(AF_INET6, ip, &in6) != -1) {
        memcpy(rule->ip6, in6.s6_addr, NHM_LEN_IPv6);
        entry = true;
      } else
        QMessageBox::warning(this, "NHM", "Invalid IP address!");
    } else
      entry = true;
  }
  if(!ui->macLineEdit->text().isEmpty()) {
    QString s = ui->macLineEdit->text();
    if(s.size() != 17)
      QMessageBox::warning(this, "NHM", "Invalid MAC address length!");
    else {
      /* test the mac addr validity */
      for(i = 0; i != s.size(); i++)
        if(!isxdigit(s.at(i).toLatin1()) && isxdigit(s.at(i).toLatin1()) != ':') break;
      if(i != s.size())
        QMessageBox::warning(this, "NHM", "Invalid MAC address format!");
      else {
        sscanf(TO_CSTR(s), "%x:%x:%x:%x:%x:%x", &imac[0], &imac[1], &imac[2], &imac[3], &imac[4], &imac[5]);
        for(i = 0; i < 6; i++) rule->hw[i] = (unsigned char)imac[i];
        entry = true;
      }
    }
  }
  if(!ui->portStartLineEdit->text().isEmpty()) {
    rule->port[0] = ui->portStartLineEdit->text().toShort();
    entry = true;
  }
  if(!ui->portEndLineEdit->text().isEmpty()) {
    rule->port[1] = ui->portEndLineEdit->text().toShort();
    entry = true;
  }
  if(!ui->ethProtoLineEdit->text().isEmpty()) {
    rule->eth_proto = ui->ethProtoLineEdit->text().toShort();
    entry = true;
  }
  if(!ui->ipProtoLineEdit->text().isEmpty()) {
    rule->ip_proto = ui->ipProtoLineEdit->text().toShort();
    entry = true;
  }
  if(!entry) {
    delete rule, rule = NULL;
  }

  return rule;
}