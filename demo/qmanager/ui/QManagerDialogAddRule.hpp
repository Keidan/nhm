/**
 * @file QManagerDialogAddRule.hpp
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
#ifndef QMANAGERDIALOGADDRULE_HPP
  #define QMANAGERDIALOGADDRULE_HPP

  #include <QDialog>
  #include "ui_QManagerDialogAddRule.h"
  #include "../model/QNHM.hpp"

  namespace Ui
  {
    class QManagerDialogAddRule;
  }

  class QManagerDialogAddRule : public QDialog
  {
      Q_OBJECT

    public:
      explicit QManagerDialogAddRule(QWidget *parent = 0);
      virtual ~QManagerDialogAddRule();

      auto display() -> QNHMRule*;

    private:
      Ui::QManagerDialogAddRule *m_ui;
  };

#endif // QMANAGERDIALOGADDRULE_HPP
