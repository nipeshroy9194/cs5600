#ifndef __SME_SELECT_H__
#define  __SME_SELECT_H__
#include "sme.h"
#include "sme_def.h"
#include "list.h"

sme_mech_t *select_mech_init();

sme_fd_t *select_mech_add_fd(sme_mech_t *mech,
                     int fd,
                     fd_event_t ev,
                     sme_fd_cb cb,
                     void *cb_data);

sme_proc_t *select_mech_add_proc(sme_mech_t *mech,
                          int pid,
                          int flags,
                          sme_proc_cb cb,
                          void *cb_data);

int select_mech_loop_wait(sme_mech_t *mech);

#endif
