/**
 *******************************************************************************
 * @file QNHM.hpp
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
#ifndef QNHM_HPP
  #define QNHM_HPP

  #include <QObject>
  #include "QNHMRule.hpp"


  class QNHM : public QObject {
    Q_OBJECT

    public:
      explicit QNHM(QObject *parent = 0);
      ~QNHM();

      /**
       * @brief Open the connection with the NHM module.
       * @return The return code of the nhm_open function.
       */
      int open();

      /**
       * @brief Close the connection with the NHM module.
       * @return The return code of the nhm_close function.
       */
      void close();

      /**
       * @brief Add a new rule to the NHM module.
       * @param rule The rule to add.
       * @return The return code of the nhm_add_rule function.
       */
      int add(QNHMRule* rule);

      /**
       * @brief Read the rules from the NHM module.
       * @param rules The readed rules.
       * @return The return code of the nhm_get_rule function.
       */
      int get(QNHMRule* rule);

      /**
       * @brief Remove a rule from the NHM module.
       * @param rule The rule to remove.
       * @return The return code of the nhm_del_rule function.
       */
      int remove(QNHMRule* rule);

      /**
       * @brief Get the number of rules to read.
       * @param output The length.
       * @return The return code of the ioctl function (man ioctl)
       */
      int size(int *output);

      /**
       * @brief Reset the internal LKM loop index.
       * @return The return code of the nhm_read_rewind function.
       */
      int rewind();

    private:
      int m_fd;
  };

  Q_DECLARE_METATYPE(QNHM*)

#endif // QNHM_HPP
