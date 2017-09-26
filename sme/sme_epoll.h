#ifndef __SME_EPOLL_H__
#define __SME_EPOLL_H__
#include "sme.h"
#include "sme_def.h"
#include "list.h"

sme_mech_t *epoll_mech_init();

sme_fd_t *epoll_mech_add_fd(sme_mech_t *mech,
                     int fd,
                     fd_event_t ev,
                     sme_fd_cb cb,
                     void *cb_data);

int epoll_mech_loop_wait(sme_mech_t *mech);

#endif
