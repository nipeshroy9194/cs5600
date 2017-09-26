#ifndef __SME_DEF_H__
#define __SME_DEF_H__

#include <stdbool.h>
/*
 * fd event type
 */
enum fd_event {
    SME_READ,
    SME_WRITE,
    SME_EXCEPTION
};

struct sme_fd {
    struct sme_fd *next, *prev;
    struct sme_mech *mech;
    int fd;
    enum fd_event ev;
    sme_fd_cb cb;
    void *cb_data;
};

struct sme_proc {
    struct sme_proc *next, *prev;
    struct sme_mech *mech;
    int pid;
    int flags;
    sme_proc_cb cb;
    void *cb_data;
};

struct sme_mech {
    /* list of fd events */
    struct sme_fd *fd_events;

    /* list of proc events */
    struct sme_proc *proc_events;

    /* private data */
    void *priv_data;
};

bool sme_sequential_init(void);
bool sme_select_init();
bool sme_poll_init();

#endif //__SME_DEF_H__
