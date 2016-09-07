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

#define TO_CSTR(obj) obj.toStdString().c_str()

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


QStringList QNHMRule::toStrings() {
  QStringList list;
  for(int i = 0; i < COLUMNS_MAX - 1; ++i)
    list << toString(i);
  return list;
}


QString QNHMRule::toString(int column) {
  QString str;
  switch(column) {
    case COLUMN_DEVICE:
      return QString::fromLatin1(dev);
    case COLUMN_TYPE: 
      switch(nf_type) {
	case NHM_NF_TYPE_DROP:    return QString("DROP");
	case NHM_NF_TYPE_ACCEPT:  return QString("ACCEPT");
	case NHM_NF_TYPE_STOLEN:  return QString("STOLEN");
	case NHM_NF_TYPE_QUEUE:   return QString("QUEUE");
	case NHM_NF_TYPE_REPEAT:  return QString("REPEAT");
	default:                  return QString("STOP");
      }
    case COLUMN_DIR:
      switch(dir) {
	case NHM_DIR_INPUT:  return QString("INPUT");
	case NHM_DIR_OUTPUT: return QString("OUTPUT");
	default:             return QString("BOTH");
      }
    case COLUMN_PROTO:
      str = QString::number(eth_proto);
      if(ip_proto) {
	str.append(" - ");
	str.append(QString::number(ip_proto));
      }
      return str;
    case COLUMN_HW:
      str = "";
      str.sprintf("%02x:%02x:%02x:%02x:%02x:%02x", (int)hw[0], (int)hw[1], (int)hw[2], (int)hw[3], (int)hw[4], (int)hw[5]);
      return str;
    case COLUMN_IP: {
      str = "";
      char bytes[4];
      nhm_from_ipv4(bytes, 0, ip4);
      str.sprintf("%03d.%03d.%03d.%03d", (int)bytes[0], (int)bytes[1], (int)bytes[2], (int)bytes[3]);
      str += " (";
      str += QString("").sprintf("%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
				 ip6[0], ip6[1], ip6[2], ip6[3],
				 ip6[4], ip6[5], ip6[5], ip6[7],
				 ip6[8], ip6[9], ip6[10], ip6[11],
				 ip6[12], ip6[13], ip6[14], ip6[15]);
      str += ")";
      return str;
    }
    case COLUMN_PORT:
      str = QString::number(port[0]);
      if(port[1]) {
	str.append(":");
	str.append(QString::number(port[1]));
      }
      return str;
    case COLUMN_APPLIED:
      str = QString::number(counter);
      str.append(" - ");
      str.append(QString("").sprintf("%lld.%.9ld", (long long)last.tv_sec, last.tv_nsec));
      return str;
  }
  return QString("ERROR");
}

void QNHMRule::fromString(int column, const QString &value) {
  unsigned int imac[6];
  struct in6_addr in6;
  bool ok = false;
  long long l;
  int idx, n;
  QString str;
  switch(column) {
    case COLUMN_DEVICE: {
      memcpy(dev, TO_CSTR(value), IFNAMSIZ);
      break;
    }
    case COLUMN_TYPE: {
      if(value == "DROP") nf_type = NHM_NF_TYPE_DROP;
      else if(value == "ACCEPT") nf_type = NHM_NF_TYPE_ACCEPT;
      else if(value == "STOLEN") nf_type = NHM_NF_TYPE_STOLEN;
      else if(value == "QUEUE") nf_type = NHM_NF_TYPE_QUEUE;
      else if(value == "REPEAT") nf_type = NHM_NF_TYPE_REPEAT;
      else if(value == "STOP") nf_type = NHM_NF_TYPE_STOP;
      break;
    }
    case COLUMN_DIR: {
      if(value == "INPUT") dir = NHM_DIR_INPUT;
      else if(value == "OUTPUT") dir = NHM_DIR_OUTPUT;
      else if(value == "BOTH") dir = NHM_DIR_BOTH;
      break;
    }
    case COLUMN_PROTO: {
      idx = value.indexOf(" - ");
      if(idx == -1)  {
	ok = false;
	n = value.left(idx).toInt(&ok, 10);
	if(ok && n != eth_proto) {
	  eth_proto = n;
	  ip_proto = 0;
	}
      } else {
	ok = false;
	n = value.left(idx).toInt(&ok, 10);
	if(ok && n != eth_proto) eth_proto = n;
	ok = false;
	n = value.right(value.size() - (idx + 3)).toInt(&ok, 10);
	if(n != ip_proto) ip_proto = n;
      }
      break;
    }
    case COLUMN_HW: {
      sscanf(TO_CSTR(value), "%x:%x:%x:%x:%x:%x", &imac[0], &imac[1], &imac[2], &imac[3], &imac[4], &imac[5]);
      for(n = 0; n < 6; ++n) hw[n] = (unsigned char)imac[n];
      break;
    }
    case COLUMN_IP: {
      const char __ip6[] = NHM_NULL_IPv6;
      memcpy(ip6, __ip6, sizeof(__ip6));
      ip4 = 0;
      idx = value.indexOf(" (");
      
      QStringList ips = value.left(idx).split(".");
      ip4 = nhm_to_ipv4(ips.at(0).toInt(&ok, 10), ips.at(1).toInt(&ok, 10), ips.at(2).toInt(&ok, 10), ips.at(3).toInt(&ok, 10));
      ips.clear();
      str = value.right(value.size() - (idx + 3));
      str = value.left(value.size() - 1); // remove ')'
      inet_pton(AF_INET6, TO_CSTR(str), &in6);
      memcpy(ip6, in6.s6_addr, NHM_LEN_IPv6);
      break;
    }
    case COLUMN_PORT: {
      idx = value.indexOf(":");
      if(idx == -1)  {
	ok = false;
	n = value.left(idx).toInt(&ok, 10);
	if(ok && n != port[0]) {
	  port[0] = n;
	  port[1] = 0;
	}
      } else {
	ok = false;
	n = value.left(idx).toInt(&ok, 10);
	if(ok && n != port[0]) port[0] = n;
	ok = false;
	n = value.right(value.size() - (idx + 3)).toInt(&ok, 10);
	if(n != port[1]) port[1] = n;
      }
      break;
    }
    case COLUMN_APPLIED: {
      idx = value.indexOf(" - ");
      ok = false;
      l = value.left(idx).toLong(&ok, 10);
      if(ok && ((unsigned long)l) != counter) counter = l;
      ok = false;
      str = value.right(value.size() - (idx + 3));
      idx = str.indexOf(".");
      l = str.left(idx).toLong(&ok, 10);
      if(ok && l != last.tv_sec) last.tv_sec = l;
      l = str.right(str.size() - (idx + 1)).toLong(&ok, 10);
      if(l != last.tv_nsec) last.tv_nsec = l;
      break;
    }
  }
}
