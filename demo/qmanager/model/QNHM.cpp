/**
 *******************************************************************************
 * @file QNHM.cpp
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
#include "QNHM.hpp"


QNHM::QNHM(QObject *parent) : QObject(parent), m_fd(-1) {
}

QNHM::~QNHM() {
  this->close();
}

/**
 * @brief Open the connection with the NHM module.
 * @return The return code of the nhm_open function.
 */
int QNHM::open() {
  m_fd = ::nhm_open();
  return m_fd;
}

/**
 * @brief Close the connection with the NHM module.
 * @return The return code of the nhm_close function.
 */
void QNHM::close() {
  if(m_fd != -1) ::nhm_close(m_fd), m_fd = -1;
}

/**
 * @brief Add a new rule to the NHM module.
 * @param rule The rule to add.
 * @return The return code of the nhm_add_rule function.
 */
int QNHM::add(QNHMRule* rule) {
  struct nhm_s r;
  rule->copy(&r);
  return nhm_add_rule(m_fd, (&r));
}

/**
 * @brief Remove a rule from the NHM module.
 * @param rule The rule to remove.
 * @return The return code of the nhm_del_rule function.
 */
int QNHM::remove(QNHMRule* rule) {
  struct nhm_s r;
  rule->copy(&r);
  return nhm_del_rule(m_fd, (&r));
}

/**
 * @brief Get the number of rules to read.
 * @param output The length.
 * @return The return code of the ioctl function (man ioctl)
 */
int QNHM::size(int *output) {
  return nhm_rules_size(m_fd, output);
}

/**
 * @brief Reset the internal LKM loop index.
 * @return The return code of the nhm_read_rewind function.
 */
int QNHM::rewind() {
  return nhm_read_rewind(m_fd);
}

/**
 * @brief Read the rules from the NHM module.
 * @param rules The readed rules.
 * @return The return code of the nhm_get_rule function.
 */
int QNHM::get(QNHMRule* rule) {
  struct nhm_s r;
  nhm_init_rule((&r));
  int ret = nhm_get_rule(m_fd, (&r));
  if (ret < 0) return ret;
  memcpy(rule->dev, r.dev, IFNAMSIZ);
  rule->nf_type = r.nf_type;
  rule->dir = r.dir;
  memcpy(rule->hw, r.hw, sizeof(unsigned char) * NHM_LEN_HW);
  rule->ip4 = r.ip4;
  memcpy(rule->ip6, r.ip6, sizeof(unsigned char) * NHM_LEN_IPv6);
  memcpy(rule->port, r.port, sizeof(unsigned short) * 2);
  rule->eth_proto = r.eth_proto;
  rule->ip_proto = r.ip_proto;
  rule->counter = r.applied.counter;
  memcpy(&rule->last, &r.applied.last, sizeof(struct timespec));
  return 0;
}
