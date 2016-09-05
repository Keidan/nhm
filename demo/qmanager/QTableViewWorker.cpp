/**
 *******************************************************************************
 * @file QTableViewWorker.cpp
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
#include <QApplication>
#include "QTableViewWorker.hpp"
#include <QDebug>


QTableViewWorker::QTableViewWorker(QObject *parent) : QObject(parent) {

}

QTableViewWorker::~QTableViewWorker() {
}

void  QTableViewWorker::doWork() {
  qDebug() << "doWork running:" << m_running << ", stopped: " << m_stopped;
  QNHMRule rule;
  emit updateRule(rule);

  if ( !m_running || m_stopped ) {
    if(!m_stopped_emit) {
      m_stopped_emit = true;
      emit stopped();
    }
    return;
  }
  sleep(1);
  // do important work here

  // allow the thread's event loop to process other events before doing more "work"
  // for instance, your start/stop signals from the MainWindow
  QMetaObject::invokeMethod(this, "doWork", Qt::QueuedConnection);
}

void QTableViewWorker::stop(QThread *owner) {
  m_stopped = true;
  m_running = false;
  qDebug() << "workStop";
  if(!m_stopped_emit) {
    m_stopped_emit = true;
    emit stopped();
  }
  owner->quit();
}

void QTableViewWorker::start(QThread *owner) {
  m_stopped_emit = false;
  m_stopped = false;
  m_running = true;
  doWork();
  qDebug() << "workStart";
  emit running();
  owner->start();
}
