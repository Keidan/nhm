/**
 * @file QTableModel.hpp
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
#ifndef QTABLEMODEL_HPP
  #define QTABLEMODEL_HPP

  #include <QAbstractTableModel>
  #include <QModelIndex>
  #include <QVariant>
  #include <QList>
  #include "../model/QNHMRule.hpp"

  class QTableModel : public QAbstractTableModel
  {

      Q_OBJECT

    public:
      QTableModel(QObject *parent=0);

      QTableModel(QList<QNHMRule*> list, QObject *parent=0);

      auto rowCount(const QModelIndex &parent) const -> int;

      auto columnCount(const QModelIndex &parent) const -> int;

      auto data(const QModelIndex &index, int role) const -> QVariant;

      auto headerData(int section, Qt::Orientation orientation, int role) const -> QVariant;

      auto flags(const QModelIndex &index) const -> Qt::ItemFlags;

      auto setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole) -> bool;

      auto insertRows(int position, int rows, const QModelIndex &index=QModelIndex()) -> bool;

      auto removeRows(int position, int rows, const QModelIndex &index=QModelIndex()) -> bool;

      auto getList() -> QList<QNHMRule*>;

      auto getColumn(int row, int column) const -> QVariant;

      auto clear() -> void;
    private:
      QList<QNHMRule*> m_list;
  };


#endif // QTABLEMODEL_HPP
