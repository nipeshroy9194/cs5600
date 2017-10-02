#include "sme.h"
#include "list.h"
#include "sme_def.h"
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct select_data select_data_t;
#define INVALID_MAXFD -1

struct select_data {
  sme_mech_t *m;
  /* max fd */
  int max_fd;
};

void calc_maxfd(select_data_t *sa) {

  sme_fd_t *fde;
  sa->max_fd = 0;

  for (fde = sa->m->fd_events; fde; fde = fde->next) {
    if (sa->max_fd < fde->fd)
      sa->max_fd = fde->fd;
  }
}

sme_mech_t *select_mech_init() {
  sme_mech_t *mech;
  select_data_t *d;

  mech = calloc(1, sizeof(*mech));
  if (!mech)
    return NULL;

  d = calloc(1, sizeof(*d));
  if (!d)
    return NULL;
  mech->priv_data = d;

  d->m = mech;
  d->max_fd = INVALID_MAXFD;

  return mech;
}

sme_fd_t *select_mech_add_fd(sme_mech_t *mech, int fd, fd_event_t ev,
                             sme_fd_cb cb, void *cb_data) {
  sme_fd_t *fde;
  select_data_t *sd = mech->priv_data;

  fde = sme_comm_add_fd(mech, fd, ev, cb, cb_data);
  if (!fde)
    return NULL;

  if ((sd->max_fd != INVALID_MAXFD) && (fde->fd > sd->max_fd)) {
    sd->max_fd = fde->fd;
  }

  return fde;
}

sme_proc_t *select_mech_add_proc(sme_mech_t *mech, int pid, int flags,
                                 sme_proc_cb cb, void *cb_data) {
  sme_proc_t *proce;

  proce = sme_comm_add_proc(mech, pid, flags, cb, cb_data);
  if (!proce)
    return NULL;

  return proce;
}

bool select_mech_loop_select(sme_mech_t *mech) {
  int sret;
  int isset;
  sme_fd_t *fde;
  fd_set r_fds, w_fds;

  FD_ZERO(&r_fds);
  FD_ZERO(&w_fds);

  select_data_t *sd = mech->priv_data;
  if (sd->max_fd == INVALID_MAXFD) {
    calc_maxfd(sd);
  }
  for (fde = sd->m->fd_events; fde; fde = fde->next) {
    if (fde->ev == SME_READ) {
      FD_SET(fde->fd, &r_fds);
    }

    if (fde->ev == SME_WRITE) {
      FD_SET(fde->fd, &w_fds);
    }
  }

  struct timeval tv;
  /* Wait up to five seconds. */
  tv.tv_sec = 2;
  tv.tv_usec = 0;

  do {
    sret = select(sd->max_fd + 1, &r_fds, &w_fds, NULL, &tv);
  } while (0 > sret && EINTR == errno);

  if (sret == -1) {
    perror("select()");
    return false;
  }

  if (sret > 0) {
    /* FD_ISSET(0, &rfds) will be true. */
    isset = 0;
    for (fde = sd->m->fd_events; fde; fde = fde->next) {
      if ((FD_ISSET(fde->fd, &r_fds)) && (fde->ev == SME_READ)) {
        isset = 1;
      }

      if ((FD_ISSET(fde->fd, &w_fds)) && (fde->ev == SME_WRITE)) {
        isset = 1;
        ;
      }

      if (isset) {
        /*
         * Remove fde from fd_events.
         * Decrement num of alive fd events
         */
        LIST_REMOVE(sd->m->fd_events, fde);
        sd->max_fd = INVALID_MAXFD;
        /*
         * Trigger the callback
         */
        fde->cb(sd->m, fde, fde->cb_data);
        break;
      }
    }
    return true;
  }
  return true;
}

int select_mech_loop_once(sme_mech_t *mech) {

  if ((mech->proc_events) && (sme_comm_loop_proc(mech)))
    return 0;

  if ((mech->fd_events) && (select_mech_loop_select(mech)))
    return 0;

  /* No events */
  return -1;
}

int select_mech_loop_wait(sme_mech_t *mech) {
  int ret;
  /**
   * Run as much as we have events
   */
  while (sme_loop_has_events(mech)) {
    ret = select_mech_loop_once(mech);
    if (ret != 0) {
      printf("loop once failed\n");
      return ret;
    }
  }

  printf("No more events. Exiting...\n");
  return 0;
}
