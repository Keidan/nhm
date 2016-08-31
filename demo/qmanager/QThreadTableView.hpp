/**
 *******************************************************************************
 * @file QThreadTableView.hpp
 * @author Keidan
 * @date 31/08/2016
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
#ifndef QTHREADTABLEVIEW_HPP
  #define QTHREADTABLEVIEW_HPP

  #include <QThread>
  #include "QNHM.hpp"


  class QThreadTableView : public QThread {
    Q_OBJECT

    public:
      explicit QThreadTableView(QObject *parent = 0);
      ~QThreadTableView();

    signals:
      void newRule(const struct nhm_s &data);

    protected:
      void run();

  };

#endif // QTHREADTABLEVIEW_HPP
