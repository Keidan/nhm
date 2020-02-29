/**
 * @file QTableModel.cpp
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
#include "QTableModel.hpp"
#include "../model/QNHMRule.hpp"
#include <QDebug>


QTableModel::QTableModel(QObject *parent) : QAbstractTableModel(parent)
{
}

QTableModel::QTableModel(QList<QNHMRule*> list, QObject *parent) : QAbstractTableModel(parent)
{
  m_list = list;
}

auto QTableModel::rowCount(const QModelIndex &parent) const -> int
{
  Q_UNUSED(parent);
  return m_list.size();
}

auto QTableModel::columnCount(const QModelIndex &parent) const -> int
{
  Q_UNUSED(parent);
  return COLUMNS_MAX;
}

auto QTableModel::getColumn(int row, int column) const -> QVariant
{
  if (row >= m_list.size() || row < 0)
    return QVariant();
  auto r = m_list.at(row);
  if(column != COLUMN_HIDDEN)
    return r->toString(column);
  else return qVariantFromValue((void *) r);
}

auto QTableModel::data(const QModelIndex &index, int role) const -> QVariant
{
  if (!index.isValid())
    return QVariant();

  if (index.row() >= m_list.size() || index.row() < 0)
    return QVariant();

  if (role == Qt::DisplayRole)
  {
    auto r = m_list.at(index.row());
    if(index.column() != COLUMN_HIDDEN)
      return r->toString(index.column());
    else return qVariantFromValue((void *) r);
  }
  return QVariant();
}

auto QTableModel::headerData(int section, Qt::Orientation orientation, int role) const -> QVariant
{
  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Horizontal)
  {
    switch (section)
    {
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
      case COLUMN_HIDDEN:
        return "Hidden";
      default:
        return QVariant();
    }
  }
  return QVariant();
}

auto QTableModel::insertRows(int position, int rows, const QModelIndex &index) -> bool
{
  Q_UNUSED(index);
  beginInsertRows(QModelIndex(), position, position+rows-1);

  for (int row=0; row < rows; row++)
  {
    auto r = new QNHMRule;
    m_list.insert(position, r);
  }

  endInsertRows();
  return true;
}

auto QTableModel::removeRows(int position, int rows, const QModelIndex &index) -> bool
{
  Q_UNUSED(index);
  beginRemoveRows(QModelIndex(), position, position+rows-1);

  for (auto row = 0; row < rows; ++row)
  {
    delete m_list.at(position);
    m_list.removeAt(position);
  }
  endRemoveRows();
  return true;
}

auto QTableModel::setData(const QModelIndex &index, const QVariant &value, int role) -> bool
{
  Q_UNUSED(role);
  if (index.isValid())
  {
    auto row = index.row();
    auto r = m_list.value(row);
    if(index.column() != COLUMN_HIDDEN)
    {
      r->fromString(index.column(), value.toString());
      m_list.replace(row, r);
      emit(dataChanged(index, index));
      return true;
    }
  }
  return false;
}

auto QTableModel::flags(const QModelIndex &index) const -> Qt::ItemFlags
{
  if (!index.isValid())
    return Qt::ItemIsEnabled;
  return QAbstractTableModel::flags(index);
}

auto QTableModel::getList() -> QList<QNHMRule*>
{
  return m_list;
}

auto QTableModel::clear() -> void
{
  this->beginResetModel();
  for (auto row = 0; row < m_list.size(); ++row)
    delete m_list.at(row);
  m_list.clear();
  this->endResetModel();
}
