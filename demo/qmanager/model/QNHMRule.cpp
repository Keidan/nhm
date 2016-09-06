/**
 *******************************************************************************
 * @file QNHMRule.cpp
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
#include "QNHMRule.hpp"

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

