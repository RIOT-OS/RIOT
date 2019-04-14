/*
 * Copyright (C) 2019 Robin Lösch <robin@chilio.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_syslog
 * @{
 *
 * @file
 * @brief       Syslog module implementation
 *
 * @author      Robin Lösch <robin@chilio.net>
 *
 * @}
 */

#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#ifdef CPU_NATIVE
#include <netdb.h>
#endif /* CPU_NATIVE */


#define ENABLE_DEBUG (0)
#include "debug.h"

#ifdef MODULE_GNRC_NETIF
#include "net/gnrc/netif.h"
#endif

#include "syslog/syslog.h"
#include "mutex.h"

#ifdef CPU_NATIVE
static char hostname[HOST_NAME_MAX];
#endif /* CPU_NATIVE */

static char addr_str[INET6_ADDRSTRLEN];

static mutex_t syslog_mutex = MUTEX_INIT;
static int cfree = -1;
kernel_pid_t syslog_pid = -1;

syslog_client_t client[SYSLOG_CLIENTS_NUMOF];

static int get_pri_numeric(int facility, int priority) {
  return ((facility << 3) + priority);
}

static char *syslog_get_addr(void) {
#ifdef MODULE_GNRC_NETIF
   for (gnrc_netif_t *iface = gnrc_netif_iter(NULL); iface != NULL; iface = gnrc_netif_iter(iface))
    {
      for (int e = 0; e < GNRC_NETIF_IPV6_ADDRS_NUMOF;e++) {
        if ((ipv6_addr_is_link_local(&iface->ipv6.addrs[e]))) {
          ipv6_addr_to_str(addr_str, &iface->ipv6.addrs[e], sizeof(addr_str));
          uint8_t ipv6_addrs_flags = iface->ipv6.addrs_flags[e];
          if ((ipv6_addrs_flags & GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_MASK) == GNRC_NETIF_IPV6_ADDRS_FLAGS_STATE_VALID)
          {
            return addr_str;
          }
        }
      }
    }
#endif
    return SYSLOG_NILVALUE;
}

static char *syslog_get_hostname(void) {
#ifdef CPU_NATIVE
  if(gethostname(hostname, sizeof(hostname)) == 0) {
    struct hostent *host = gethostbyname(hostname);
    if(host) {
      return host->h_name;
    }
  } else {
    return syslog_get_addr();
  }
#else /* !CPU_NATIVE */
  return syslog_get_addr();
#endif
  return SYSLOG_NILVALUE;
}

static syslog_client_t *get_client(kernel_pid_t pid) {
  for (int i = 0; i < SYSLOG_CLIENTS_NUMOF;i++) {
    if(client[i].pid == pid) {
      return &client[i];
    }
  }
  return NULL;
}

static int syslog_msg_send(msg_t *m, kernel_pid_t target_pid) {
DEBUG("syslog_msg_send: Send ");
#ifdef SYSLOG_SEND_NONBLOCK
  DEBUG("non-blocking\n");
  return msg_try_send(m, target_pid);
#else
  DEBUG("blocking\n");
  return msg_send(m, target_pid);
#endif
}


void openlog (const char *ident, int option, int facility) {
  syslog_client_t *c = NULL;

  if (ident == NULL)
  {
    DEBUG("openlog: ident is NULL\n");
    return;
  }

  if(cfree == 0) {
    DEBUG("openlog: Error maximum clients number %d already used\n", SYSLOG_CLIENTS_NUMOF);
    return;
  }

  if (cfree == -1) {
    DEBUG("openlog: Data structs not initzialized yet\n");
    memset(client, 0, sizeof(client));
    cfree = 5;
    c = &client[0];
  } else {
    c = get_client(thread_getpid());
  }

  if(!c) {
    for (int i = 0; i < SYSLOG_CLIENTS_NUMOF; i++)
    {
      if(client[i].pid == 0) {
        DEBUG("openlog: New client %d\n",i);
        c = &client[i];
        break;
      }
    }
    cfree--;
  }

  if(!c) {
    assert(false);
  }
  c->opt = option;
  c->pid = thread_getpid();
  if (OPT_MASK_TEST(c->opt, LOG_PID)) {
    snprintf(c->ident, IDENT_MAX_LEN, "%s[%d]",ident, c->pid);
  } else {
    strncpy(c->ident, ident, IDENT_MAX_LEN);
  }
  c->ident[IDENT_MAX_LEN-1] = 0;
  c->facility = facility;
  c->mask     = LOG_MASK_ALL;
  DEBUG("openlog: New client with pid %d facility %d and ident \"%s\"\n", c->pid, c->facility, c->ident);
  return;
}

void syslog(int facility_priority, const char *format, ...) {
  va_list ap;
  va_start(ap,format);
  vsyslog(facility_priority, format, ap);
  va_end(ap);
  return;
}

void vsyslog(int facility_priority, const char *format, va_list ap) {
  int len = 0, i;

  if(cfree < 0) {
    DEBUG("vsyslog: syslog not initialized\n");
    return;
  }
  syslog_client_t *c = get_client(thread_getpid());
  if(!c) {
    DEBUG("vsyslog: No client found for pid %d\n",(int)thread_getpid());
    return;
  }
  if(!LOG_MASK_TEST(c->mask,facility_priority)) {
    DEBUG("vsyslog: LOG_MASK %d bit for %d not set\n",c->mask,LOG_MASK(facility_priority));
    return;
  }
  mutex_lock(&syslog_mutex);
  i = snprintf(c->buf, SYSLOG_MAX_LEN, "<%d> %d - %s %s %d %d ", get_pri_numeric(c->facility, facility_priority), SYSLOG_VERSION, syslog_get_hostname(), c->ident, 0, 0);
  if(i<0) {
    DEBUG("vsyslog: Encoding error on writing the header\n");
    mutex_unlock(&syslog_mutex);
    return;
  }
  len += (i < SYSLOG_MAX_LEN) ? i : SYSLOG_MAX_LEN;

  i = vsnprintf((c->buf) + len, (SYSLOG_MAX_LEN - len), format, ap);
  if(i<0) {
    DEBUG("vsyslog: Encoding error on writing the message content\n");
    mutex_unlock(&syslog_mutex);
    return;
  }
  len += (i < (SYSLOG_MAX_LEN-len)) ? i : (SYSLOG_MAX_LEN-len);
  assert(len <= SYSLOG_MAX_LEN);

  if (syslog_pid > 0) {
    msg_t msg;
    c->msg.len = len;
    c->msg.ptr = c->buf;
    msg.content.ptr = &c->msg;
    DEBUG("vsyslog: Try to send syslog msg with %d bytes content\n", c->msg.len);

    if (syslog_msg_send(&msg, syslog_pid) == 0)
    {
      DEBUG("vsyslog: Syslog receiver unable to receive message\n");
    }
  } else {
    if(OPT_MASK_TEST(c->opt,LOG_CONS)) {
      printf("%s\n", c->buf);
    }
    DEBUG("vsyslog: No receiver registered");
  }
  if(OPT_MASK_TEST(c->opt,LOG_PERROR)) {
    fprintf(stderr,"%s\n", c->buf);
  }
  mutex_unlock(&syslog_mutex);
  return;
}

void  closelog (void) {
  syslog_client_t *c = get_client(thread_getpid());
  if(!c) {
    return;
  }
  c->facility = -1;
  c->pid = 0;
  memset(c->ident, 0, sizeof(c->ident));
  cfree++;
  return;
}

int setlogmask(int mask) {
  syslog_client_t *c = get_client(thread_getpid());
  if(!c) {
    DEBUG("setlogmask: No client with pid %d\n", (int)thread_getpid());
    return -1;
  }
  int old_mask = c->mask;
  if(mask > 0) {
    DEBUG("setlogmask: Set mask to %d\n", mask);
    c->mask = mask;
  }
  return old_mask;
}