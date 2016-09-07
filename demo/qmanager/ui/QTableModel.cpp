/**
*******************************************************************************
* @file QTableModel.cpp
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
#include "QTableModel.hpp"
#include "../model/QNHMRule.hpp"
#include <QDebug>


QTableModel::QTableModel(QObject *parent) : QAbstractTableModel(parent) {
}

QTableModel::QTableModel(QList<QNHMRule*> list, QObject *parent) : QAbstractTableModel(parent) {
  m_list = list;
}

int QTableModel::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return m_list.size();
}

int QTableModel::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return COLUMNS_MAX;
}

QVariant QTableModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  if (index.row() >= m_list.size() || index.row() < 0)
    return QVariant();

  if (role == Qt::DisplayRole) {
    QNHMRule *r = m_list.at(index.row());
    return r->toString(index.column());
  }
  return QVariant();
}

QVariant QTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Horizontal) {
    switch (section) {
      case COLUMN_DEVICE:
        return "Device";
      case COLUMN_TYPE:
        return "Type";
      case COLUMN_DIR:
        return "Dir";
      case COLUMN_PROTO:
        return "Proto";
      case COLUMN_HW:
        return "HW";
      case COLUMN_IP:
        return "IP";
      case COLUMN_PORT:
        return "PORT";
      case COLUMN_APPLIED:
        return "Applied";
      default:
        return QVariant();
    }
  }
  return QVariant();
}

bool QTableModel::insertRows(int position, int rows, const QModelIndex &index) {
  Q_UNUSED(index);
  beginInsertRows(QModelIndex(), position, position+rows-1);

  for (int row=0; row < rows; row++) {
    QNHMRule *r = new QNHMRule;
    m_list.insert(position, r);
  }

  endInsertRows();
  return true;
}

bool QTableModel::removeRows(int position, int rows, const QModelIndex &index) {
  Q_UNUSED(index);
  beginRemoveRows(QModelIndex(), position, position+rows-1);

  for (int row=0; row < rows; ++row) {
    delete m_list.at(position);
    m_list.removeAt(position);
  }
  endRemoveRows();
  return true;
}

bool QTableModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  Q_UNUSED(role);
  if (index.isValid()) {
    int row = index.row();
    QNHMRule *r = m_list.value(row);
    r->fromString(index.column(), value.toString());
    m_list.replace(row, r);
    emit(dataChanged(index, index));
    return true;
  }
  return false;
}

Qt::ItemFlags QTableModel::flags(const QModelIndex &index) const {
  if (!index.isValid())
    return Qt::ItemIsEnabled;
  return QAbstractTableModel::flags(index);
}

QList<QNHMRule*> QTableModel::getList() {
  return m_list;
}

void QTableModel::clear(){
   this->beginResetModel();
   for (int row=0; row < m_list.size(); ++row)
     delete m_list.at(row);
   m_list.clear();
   this->endResetModel();
}
