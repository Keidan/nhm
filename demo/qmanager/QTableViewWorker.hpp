/**
 *******************************************************************************
 * @file QTableViewWorker.hpp
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
#ifndef QTABLEVIEWWORKER_HPP
  #define QTABLEVIEWWORKER_HPP

  #include <QObject>
  #include <QThread>
  #include "QNHM.hpp"


  class QTableViewWorker : public QObject {
    Q_OBJECT

    public:
      explicit QTableViewWorker(QObject *parent = 0);
      ~QTableViewWorker();

      void stop(QThread *owner);
      void start(QThread *owner);

    signals:
      void updateRule(const QNHMRule &data);
      void stopped();
      void running();

    private slots:
      void doWork();

    private:
      volatile bool m_running;
      volatile bool m_stopped;
      bool m_stopped_emit;
  };

#endif // QTABLEVIEWWORKER_HPP
