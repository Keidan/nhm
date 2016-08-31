/**
 *******************************************************************************
 * @file QThreadTableView.cpp
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
#include "QThreadTableView.hpp"
#include <QDebug>


QThreadTableView::QThreadTableView(QObject *parent) : QThread(parent) {
}

QThreadTableView::~QThreadTableView() {
  exit();
}

void QThreadTableView::run() {
  qDebug() << Q_FUNC_INFO << QThread::currentThreadId();
  struct nhm_s rule;
  emit newRule(rule);
  // On lance la boucle d'événements (nécessaire pour gérer les signaux). 
  exec();
}


void QThreadTableView::stop() {
  quit();
  exit();
}
