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

QNHMRule::QNHMRule(QObject *parent) : QObject(parent) {
  memset(dev, 0, IFNAMSIZ);
  nf_type = NHM_NF_TYPE_ACCEPT;
  dir = NHM_DIR_BOTH;
  ip4 = 0;
  memset(hw, 0, NHM_LEN_HW);
  memset(ip6, 0, NHM_LEN_IPv6);
  port[0] = port[1] = eth_proto = ip_proto = 0;
  counter = 0;
  memset(&last, 0, sizeof(struct timespec));
}

QNHMRule::~QNHMRule() {
}

void QNHMRule::copy(void* api_rule) {
  struct nhm_s* r = static_cast<struct nhm_s*>(api_rule);
  nhm_init_rule(r);
  memcpy(r->dev, dev, IFNAMSIZ);
  r->nf_type = nf_type;
  r->dir = dir;
  memcpy(r->hw, hw, sizeof(unsigned char) * NHM_LEN_HW);
  r->ip4 = ip4;
  memcpy(r->ip6, ip6, sizeof(unsigned char) * NHM_LEN_IPv6);
  memcpy(r->port, port, sizeof(unsigned short) * 2);
  r->eth_proto = eth_proto;
  r->ip_proto = ip_proto;
  r->applied.counter = counter;
  memcpy(&r->applied.last, &last, sizeof(struct timespec));
}


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

