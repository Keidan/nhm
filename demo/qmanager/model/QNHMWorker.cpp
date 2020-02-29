/**
 * @file QNHMWorker.cpp
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
#include <QApplication>
#include "QNHMWorker.hpp"
#include <errno.h>

QNHMWorker::QNHMWorker(QNHM &nhm, QObject *parent) : QObject(parent), m_nhm(nhm)
{

}

QNHMWorker::~QNHMWorker()
{

}

auto QNHMWorker::doWork() -> void
{
  /* number of rules */
  int length = 0;
  auto ret = m_nhm.size(length);
  if(ret != -1)
  {
    m_nhm.rewind();
    emit clearRule();
    /* read the rules from the LKM */
    for(int i = 0; i < length; i++)
    {
      auto rule = new QNHMRule;
      /* Read the response from the LKM */
      ret = m_nhm.get(rule);
      if (ret < 0)
      {
        delete rule;
        /* Display the error message */
        QString err = "Failed to read the message from the LKM: ";
        err.append(strerror(errno));
        emit error(err);
        break;
      }
      emit updateRule(rule);
      delete rule;
    }
  }
  else
  {
    /* Display the error message */
    QString err = "Unable to read the number of rules: ";
    err.append(strerror(errno));
    emit error(err);
  }

  if ( !m_running || m_stopped )
  {
    if(!m_stopped_emit)
    {
      m_stopped_emit = true;
      emit stopped();
    }
    return;
  }
  sleep(3);
  // do important work here

  // allow the thread's event loop to process other events before doing more "work"
  // for instance, your start/stop signals from the MainWindow
  QMetaObject::invokeMethod(this, "doWork", Qt::QueuedConnection);
}

auto QNHMWorker::stop(QThread *owner) -> void
{
  m_stopped = true;
  m_running = false;
  if(!m_stopped_emit)
  {
    m_stopped_emit = true;
    emit stopped();
  }
  owner->quit();
}

auto QNHMWorker::start(QThread *owner) -> void
{
  m_stopped_emit = false;
  m_stopped = false;
  m_running = true;
  doWork();
  emit running();
  owner->start();
}
