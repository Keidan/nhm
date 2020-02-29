/**
 * @file QNHM.hpp
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
#ifndef QNHM_HPP
  #define QNHM_HPP

  #include <QObject>
  #include "QNHMRule.hpp"


  class QNHM : public QObject
  {
      Q_OBJECT

    public:
      explicit QNHM(QObject *parent = 0);
      virtual ~QNHM();

      /**
       * @brief Open the connection with the NHM module.
       * @return The return code of the nhm_open function.
       */
      auto open() -> int;

      /**
       * @brief Close the connection with the NHM module.
       * @return The return code of the nhm_close function.
       */
      auto close() -> void;

      /**
       * @brief Add a new rule to the NHM module.
       * @param rule The rule to add.
       * @return The return code of the nhm_add_rule function.
       */
      auto add(QNHMRule *rule) -> int;

      /**
       * @brief Read the rules from the NHM module.
       * @param rules The readed rules.
       * @return The return code of the nhm_get_rule function.
       */
      auto get(QNHMRule *rule) -> int;

      /**
       * @brief Remove a rule from the NHM module.
       * @param rule The rule to remove.
       * @return The return code of the nhm_del_rule function.
       */
      auto remove(QNHMRule *rule) -> int;

      /**
       * @brief Get the number of rules to read.
       * @param output The length.
       * @return The return code of the ioctl function (man ioctl)
       */
      auto size(int &output) -> int;

      /**
       * @brief Reset the internal LKM loop index.
       * @return The return code of the nhm_read_rewind function.
       */
      auto rewind() -> int;

    private:
      int m_fd;
  };

  Q_DECLARE_METATYPE(QNHM*)

#endif // QNHM_HPP
