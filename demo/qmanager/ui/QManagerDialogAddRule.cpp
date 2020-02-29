/**
 * @file QManagerDialogAddRule.cpp
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
#include "QManagerDialogAddRule.hpp"
#include <QMessageBox>
#include <QIntValidator>
#include <ctype.h>
#include <limits>


#define TO_CSTR(obj) obj.toStdString().c_str()

QManagerDialogAddRule::QManagerDialogAddRule(QWidget *parent) :
  QDialog(parent),
  m_ui(new Ui::QManagerDialogAddRule)
{

  m_ui->setupUi(this);
  setModal(true);

  m_ui->portStartLineEdit->setValidator(new QIntValidator(0, 65535, this));
  m_ui->portEndLineEdit->setValidator(new QIntValidator(0, 65535, this));
  auto max = std::numeric_limits<int>::max();
  m_ui->ethProtoLineEdit->setValidator(new QIntValidator(0, max, this));
  m_ui->ipProtoLineEdit->setValidator(new QIntValidator(0, max, this));
}

QManagerDialogAddRule::~QManagerDialogAddRule()
{
  delete m_ui;
}


auto QManagerDialogAddRule::display() -> QNHMRule*
{
  foreach(QLineEdit *widget, this->findChildren<QLineEdit*>())
    widget->clear();
  exec();
  struct in_addr in4;
  struct in6_addr in6;
  int i;
  auto entry = false;
  auto rule = new QNHMRule;

  /* add name */
  if(!m_ui->devnameLineEdit->text().isEmpty())
  {
    rule->fromString(COLUMN_DEVICE, m_ui->devnameLineEdit->text());
    entry = true;
  }

  /* add direction */
  auto text = m_ui->directionCombobox->currentText();
  rule->fromString(COLUMN_DIR, text);

  /* add nf type */
  text = m_ui->nfTypeCombobox->currentText();
  rule->fromString(COLUMN_TYPE, text);

  /* convert ip */
  if(!m_ui->ipLineEdit->text().isEmpty())
  {
    const char* ip = TO_CSTR(m_ui->ipLineEdit->text());
    if(inet_pton(AF_INET, ip, &in4) == -1)
    {
      if(inet_pton(AF_INET6, ip, &in6) != -1)
      {
        memcpy(rule->ip6, in6.s6_addr, NHM_LEN_IPv6);
        entry = true;
      }
      else
        QMessageBox::warning(this, "NHM", "Invalid IP address!");
    }
    else
    {
      entry = true;
      rule->ip4 = in4.s_addr;
    }
  }
  if(!m_ui->macLineEdit->text().isEmpty())
  {
    QString s = m_ui->macLineEdit->text();
    if(s.size() != 17)
      QMessageBox::warning(this, "NHM", "Invalid MAC address length!");
    else
    {
      /* test the mac addr validity */
      for(i = 0; i != s.size(); i++)
        if(!isxdigit(s.at(i).toLatin1()) && s.at(i).toLatin1() != ':') break;
      if(i != s.size())
        QMessageBox::warning(this, "NHM", "Invalid MAC address format!");
      else
      {
        rule->fromString(COLUMN_HW, s);
        entry = true;
      }
    }
  }
  if(!m_ui->portStartLineEdit->text().isEmpty() || !m_ui->portEndLineEdit->text().isEmpty())
  {
    auto str = m_ui->portStartLineEdit->text();
    if(!m_ui->portEndLineEdit->text().isEmpty())
    {
      str.append(":").append(m_ui->portEndLineEdit->text());
    }
    rule->fromString(COLUMN_PORT, str);
    entry = true;
  }

  if(!m_ui->ethProtoLineEdit->text().isEmpty() || !m_ui->ipProtoLineEdit->text().isEmpty())
  {
    auto str = m_ui->ethProtoLineEdit->text();
    if(!m_ui->ipProtoLineEdit->text().isEmpty())
    {
      str.append(" - ").append(m_ui->ipProtoLineEdit->text());
    }
    rule->fromString(COLUMN_PROTO, str);
    entry = true;
  }
  if(!entry)
  {
    delete rule, rule = NULL;
  }

  return rule;
}
