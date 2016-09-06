/**
 *******************************************************************************
 * @file QNHMWorker.hpp
 * @author Keidan
 * @date 06/09/2016
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
#ifndef QNHMWORKER_HPP
  #define QNHMWORKER_HPP

  #include <QObject>
  #include <QThread>
  #include <QString>
  #include "QNHM.hpp"


  class QNHMWorker : public QObject {
    Q_OBJECT

    public:
      explicit QNHMWorker(QNHM *nhm, QObject *parent = 0);
      ~QNHMWorker();

      void stop(QThread *owner);
      void start(QThread *owner);

    signals:
      void updateRule(const QNHMRule &data);
      void clearRule();
      void error(const QString &message);
      void stopped();
      void running();

    private slots:
      void doWork();

    private:
      QNHM *m_nhm;
      volatile bool m_running;
      volatile bool m_stopped;
      bool m_stopped_emit;
  };

#endif // QNHMWORKER_HPP
