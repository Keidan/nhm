/**
 * @file QNHMWorker.hpp
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
#ifndef QNHMWORKER_HPP
  #define QNHMWORKER_HPP

  #include <QObject>
  #include <QThread>
  #include <QString>
  #include "QNHM.hpp"


  class QNHMWorker : public QObject
  {
      Q_OBJECT

    public:
      explicit QNHMWorker(QNHM &nhm, QObject *parent = 0);
      virtual ~QNHMWorker();

      auto stop(QThread *owner) -> void;
      auto start(QThread *owner) -> void;

    signals:
      void updateRule(QNHMRule* data);
      void clearRule();
      void error(const QString &message);
      void stopped();
      void running();

    private slots:
      void doWork();

    private:
      QNHM &m_nhm;
      volatile bool m_running;
      volatile bool m_stopped;
      bool m_stopped_emit;
  };

#endif // QNHMWORKER_HPP
