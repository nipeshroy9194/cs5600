#include "sme.h"
#include "list.h"
#include "sme_def.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

sme_fd_t *sme_comm_add_fd(sme_mech_t *mech, int fd, fd_event_t ev, sme_fd_cb cb,
                          void *cb_data) {
  sme_fd_t *fde;

  if (fd < 0)
    return NULL;

  fde = calloc(1, sizeof(*fde));
  if (fde == NULL) {
    return NULL;
  }

  fde->mech = mech;
  fde->fd = fd;
  fde->ev = ev;
  fde->cb = cb;
  fde->cb_data = cb_data;

  /* Add to fds to the events list */

  LIST_ADD_END(mech->fd_events, fde);

  return fde;
}

sme_proc_t *sme_comm_add_proc(sme_mech_t *mech, int pid, int flags,
                              sme_proc_cb cb, void *cb_data) {
  sme_proc_t *proce;

  if (pid < 0)
    return NULL;

  proce = calloc(1, sizeof(*proce));
  if (proce == NULL) {
    return NULL;
  }

  proce->mech = mech;
  proce->pid = pid;
  proce->flags = flags;
  proce->cb = cb;
  proce->cb_data = cb_data;

  /* Add to pid to the events list */

  LIST_ADD_END(mech->proc_events, proce);

  return proce;
}

bool sme_comm_loop_proc(sme_mech_t *mech) {
  int ws;
  int ret;
  sme_proc_t *proce;
  sme_proc_cb proc_cb;
  void *cb_data;

  proce = mech->proc_events;
  if (!proce)
    return false;

  for (; proce; proce = proce->next) {
    ret = waitpid(proce->pid, &ws, proce->flags);
    if (ret == -1) {
      printf("wait failed");
      return false;
    } else if (WIFEXITED(ws)) {

      break;
    }
  }

  if (!proce)
    /* No exit */
    return true;

  proc_cb = proce->cb;
  cb_data = proce->cb_data;

  LIST_REMOVE(mech->proc_events, proce);

  /**
   * Trigger proc event callback
   */
  proc_cb(mech, proce, cb_data);

  return true;
}

bool sme_loop_has_events(sme_mech_t *mech) {
  return ((mech->fd_events != NULL) || (mech->proc_events != NULL));
}
