/**
 *******************************************************************************
 * @file QNHMWorker.cpp
 * @author Keidan
 * @date 06/092016
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
#include <QApplication>
#include "QNHMWorker.hpp"
#include <errno.h>

QNHMWorker::QNHMWorker(QNHM *nhm, QObject *parent) : QObject(parent), m_nhm(nhm) {

}

QNHMWorker::~QNHMWorker() {
}

void  QNHMWorker::doWork() {
  /* number of rules */
  int length = 0;
  int ret = m_nhm->size(&length);
  if(ret != -1) {
    m_nhm->rewind();
    emit clearRule();
    /* read the rules from the LKM */
    for(int i = 0; i < length; i++) {
      QNHMRule *rule = new QNHMRule;
      /* Read the response from the LKM */
      ret = m_nhm->get(rule);
      if (ret < 0) {
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
  } else {
    /* Display the error message */
    QString err = "Unable to read the number of rules: ";
    err.append(strerror(errno));
    emit error(err);
  }

  if ( !m_running || m_stopped ) {
    if(!m_stopped_emit) {
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

void QNHMWorker::stop(QThread *owner) {
  m_stopped = true;
  m_running = false;
  if(!m_stopped_emit) {
    m_stopped_emit = true;
    emit stopped();
  }
  owner->quit();
}

void QNHMWorker::start(QThread *owner) {
  m_stopped_emit = false;
  m_stopped = false;
  m_running = true;
  doWork();
  emit running();
  owner->start();
}
