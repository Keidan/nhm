/**
 *******************************************************************************
 * @file QNHMRule.hpp
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
#ifndef QNHMRULE_HPP
  #define QNHMRULE_HPP

  #include <QObject>
  #include <QString>
  #include <QStringList>
  #include <nhm.h>

  #define COLUMN_DEVICE  0
  #define COLUMN_TYPE    1
  #define COLUMN_DIR     2
  #define COLUMN_PROTO   3
  #define COLUMN_HW      4
  #define COLUMN_IP      5
  #define COLUMN_PORT    6
  #define COLUMN_APPLIED 7
  #define COLUMN_HIDDEN  8
  #define COLUMNS_MAX    8
 
  class QNHMRule : public QObject {
    Q_OBJECT

    public:
      explicit QNHMRule(QObject *parent = 0);
      ~QNHMRule();

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

     void copy(void* api_rule);

     QStringList toStrings();
     QString toString(int column);
     void fromString(int column, const QString &value);

  };
  Q_DECLARE_METATYPE(QNHMRule*)


#endif // QNHMRULE_HPP
