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
int QNHM::add(struct nhm_s* rule) {
  return nhm_add_rule(m_fd, rule);
}

/**
 * @brief Remove a rule from the NHM module.
 * @param rule The rule to remove.
 * @return The return code of the nhm_del_rule function.
 */
int QNHM::remove(struct nhm_s* rule) {
  return nhm_del_rule(m_fd, rule);
}

/**
 * @brief Set all fields to 0.
 * @param rule The rule to reset.
 */
void QNHM::resetRule(struct nhm_s* rule) {
  nhm_init_rule(rule);
}
