/**
 *******************************************************************************
 * @file QManagerDialogAddRule.hpp
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
#ifndef QMANAGERDIALOGADDRULE_HPP
  #define QMANAGERDIALOGADDRULE_HPP

  #include <QDialog>
  #include "ui_QManagerDialogAddRule.h"
  #include "nhm.h"

  namespace Ui {
    class QManagerDialogAddRule;
  }

  class QManagerDialogAddRule : public QDialog {
    Q_OBJECT

    public:
      explicit QManagerDialogAddRule(QWidget *parent = 0);
      ~QManagerDialogAddRule();

      struct nhm_s* display();

    private:
      Ui::QManagerDialogAddRule *ui;
  };

#endif // QMANAGERDIALOGADDRULE_HPP
