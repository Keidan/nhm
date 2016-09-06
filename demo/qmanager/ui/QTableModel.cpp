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

#define TO_CSTR(obj) obj.toStdString().c_str()

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
  return 6;
}

QVariant QTableModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  if (index.row() >= m_list.size() || index.row() < 0)
    return QVariant();

  if (role == Qt::DisplayRole) {
    QNHMRule *r = m_list.at(index.row());

    if (index.column() == 0)
      return QString(r->dev);
    else if (index.column() == 1) {
      switch(r->nf_type) {
	case NHM_NF_TYPE_DROP:
	  return QString("DROP");
	case NHM_NF_TYPE_ACCEPT:
	  return QString("ACCEPT");
	case NHM_NF_TYPE_STOLEN:
	  return QString("STOLEN");
	case NHM_NF_TYPE_QUEUE:
	  return QString("QUEUE");
	case NHM_NF_TYPE_REPEAT:
	  return QString("REPEAT");
	default:
	  return QString("STOP");
      }
    } else if (index.column() == 2) {
      switch(r->nf_type) {
	case NHM_DIR_INPUT:
	  return QString("INPUT");
	case NHM_DIR_OUTPUT:
	  return QString("OUTPUT");
	default:
	  return QString("BOTH");

      }
    } else if (index.column() == 3) {
      QString str = QString::number(r->eth_proto);
      if(r->ip_proto) {
	str.append(" - ");
	str.append(QString::number(r->ip_proto));
      }
      return str;
    } else if (index.column() == 4) {
      QString str = "";
      str.sprintf("%02x:%02x:%02x:%02x:%02x:%02x", (int)r->hw[0], (int)r->hw[1], (int)r->hw[2], (int)r->hw[3], (int)r->hw[4], (int)r->hw[5]);
      return str;
    } else if (index.column() == 5) {
      char bytes[4];
      nhm_from_ipv4(bytes, 0, r->ip4);
      return QString("").sprintf("%03d.%03d.%03d.%03d", (int)bytes[0], (int)bytes[1], (int)bytes[2], (int)bytes[3]);
    } else if (index.column() == 6) {
      QString str = "Count: ";
      str.append(QString::number(r->counter));
      str.append("\nLast: ");
      str.append(QString("").sprintf("%lld.%.9ld", (long long)r->last.tv_sec, r->last.tv_nsec));
      return str;
    }
  }
  return QVariant();
}

QVariant QTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Horizontal) {
    switch (section) {
      case 0:
        return "Device";
      case 1:
        return "Type";
      case 2:
        return "Dir";
      case 3:
        return "Proto";
      case 4:
        return "HW";
      case 5:
        return "IP";
      case 6:
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
  if (index.isValid() && role == Qt::EditRole) {
    int row = index.row();
    QNHMRule *r = m_list.value(row);

    qDebug() << "**column " << index.column() << ": " << value.toString();
    if (index.column() == 0)
      memcpy(r->dev, TO_CSTR(value.toString()), IFNAMSIZ);
    else if (index.column() == 1) {
      if(value.toString() == "DROP")
	r->nf_type = NHM_NF_TYPE_DROP;
      else if(value.toString() == "ACCEPT")
	r->nf_type = NHM_NF_TYPE_ACCEPT;
      else if(value.toString() == "STOLEN")
	r->nf_type = NHM_NF_TYPE_STOLEN;
      else if(value.toString() == "QUEUE")
	r->nf_type = NHM_NF_TYPE_QUEUE;
      else if(value.toString() == "REPEAT")
	r->nf_type = NHM_NF_TYPE_REPEAT;
      else
	r->nf_type = NHM_NF_TYPE_STOP;
    } else if (index.column() == 2) {
      if(value.toString() == "INPUT")
	r->dir = NHM_DIR_INPUT;
      else if(value.toString() == "OUTPUT")
	r->dir = NHM_DIR_OUTPUT;
      else
	r->dir = NHM_DIR_BOTH;
    } else if (index.column() == 3) {
      QString str = value.toString();
      int idx = str.indexOf(" - ");
      if(idx == -1) {
	bool ok = false;
	int number = str.toInt(&ok, 10);
	if( ok ){
	  r->eth_proto = number;
	}
      } else {
	bool ok = false;
	int number = str.left(idx).toInt(&ok, 10);
	if( ok ){
	  r->eth_proto = number;
	  number = str.right(str.size() - idx).toInt(&ok, 10);
	  if( ok ){
	    r->ip_proto = number;
	  }
	}
      }
    } else if (index.column() == 4) {
      qDebug() << "column 4: " << value.toString();
    } else if (index.column() == 5) {
      qDebug() << "column 5: " << value.toString();
    } else if (index.column() == 6) {
      qDebug() << "column 6: " << value.toString();
    }
    else
      return false;

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
