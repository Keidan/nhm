/**
 *******************************************************************************
 * @file QTableModel.hpp
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
#ifndef QTABLEMODEL_HPP
  #define QTABLEMODEL_HPP

  #include <QAbstractTableModel>
  #include <QModelIndex>
  #include <QVariant>
  #include <QList>
  #include "../model/QNHMRule.hpp"

  class QTableModel : public QAbstractTableModel {
    
    Q_OBJECT

    public:
      QTableModel(QObject *parent=0);

      QTableModel(QList<QNHMRule*> list, QObject *parent=0);

      int rowCount(const QModelIndex &parent) const;

      int columnCount(const QModelIndex &parent) const;

      QVariant data(const QModelIndex &index, int role) const;

      QVariant headerData(int section, Qt::Orientation orientation, int role) const;

      Qt::ItemFlags flags(const QModelIndex &index) const;

      bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);

      bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());

      bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());

      QList<QNHMRule*> getList();

      void clear();
    private:
      QList<QNHMRule*> m_list;
  };


#endif // QTABLEMODEL_HPP
