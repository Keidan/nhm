/**
 * @file QNHMRule.hpp
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
#ifndef QNHMRULE_HPP
  #define QNHMRULE_HPP

  #include <QObject>
  #include <QString>
  #include <QStringList>
  #include <nhm.h>

  constexpr std::uint8_t COLUMN_DEVICE  = 0;
  constexpr std::uint8_t COLUMN_TYPE    = 1;
  constexpr std::uint8_t COLUMN_DIR     = 2;
  constexpr std::uint8_t COLUMN_PROTO   = 3;
  constexpr std::uint8_t COLUMN_HW      = 4;
  constexpr std::uint8_t COLUMN_IP      = 5;
  constexpr std::uint8_t COLUMN_PORT    = 6;
  constexpr std::uint8_t COLUMN_APPLIED = 7;
  constexpr std::uint8_t COLUMN_HIDDEN  = 8;
  constexpr std::uint8_t COLUMNS_MAX    = COLUMN_HIDDEN;

  class QNHMRule : public QObject
  {
      Q_OBJECT

    public:
      explicit QNHMRule(QObject *parent = 0);
      virtual ~QNHMRule() = default;

      char            dev[IFNAMSIZ];      /*!< Network device name */
      nhm_nf_type_te  nf_type;            /*!< Netfilter policy. */
      nhm_dir_te      dir;                /*!< Packet direction */
      unsigned char   hw[NHM_LEN_HW];     /*!< Hardware MAC address. */
      unsigned int    ip4;                /*!< IPv4 address. */
      unsigned char   ip6[NHM_LEN_IPv6];  /*!< IPv address. */
      unsigned short  port[2];            /*!< Port value or range. */
      unsigned short  eth_proto;          /*!< Ethernet protocol ID */
      unsigned short  ip_proto;           /*!< IP protocol ID */
      unsigned long   counter;            /*!< Simple counter to obtain the number of times the rule is applied.*/
      struct timespec last;               /*!< Last time where the rule is applied. */

      auto copy(void* api_rule) -> void;

      auto toStrings() -> QStringList;
      auto toString(int column) -> QString;
      auto fromString(int column, const QString &value) -> void;

  };
  Q_DECLARE_METATYPE(QNHMRule*)


#endif // QNHMRULE_HPP
