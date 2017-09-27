#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/epoll.h>
#include "sme.h"
#include "sme_def.h"
#include "list.h"

typedef struct e_poll_data e_poll_data_t;

struct e_poll_data {
    sme_mech_t *m;
    /* epoll create fd */
    int epoll_fd;
};


sme_mech_t *epoll_mech_init()
{
    sme_mech_t *mech;
    e_poll_data_t *d;
	int epoll_fd = 0;

    mech = calloc(1, sizeof(*mech));
    if (!mech)
        return NULL;

    d = calloc(1, sizeof(*d));
    if (!d)
        return NULL;
    mech->priv_data = d;

	epoll_fd = epoll_create1(0);
	if (epoll_fd == -1) {
		printf("Error while epoll_create: %d\n", errno);
		exit(1);
	}

    d->m = mech;
	d->epoll_fd = epoll_fd;

    return mech;
}

sme_fd_t *epoll_mech_add_fd(sme_mech_t *mech,
                     int fd,
                     fd_event_t ev,
                     sme_fd_cb cb,
                     void *cb_data)
{
    sme_fd_t *fde;
    e_poll_data_t *ed = mech->priv_data;
	int ret = -1;
	struct epoll_event event;

	event.data.fd = fd;
	event.events = EPOLLIN;
	ret = epoll_ctl(ed->epoll_fd, EPOLL_CTL_ADD, fd, &event);
	if (ret == -1) {
		printf("Epoll ctl call failed :%d\n", errno);
		exit(1);
	}

    fde = sme_comm_add_fd(mech,
                    fd,
                    ev,
                    cb,
                    cb_data);
    return fde;
}

bool epoll_mech_loop_once(sme_mech_t *mech)
{
    int eret;
    int tv = 5000; // 5 seconds
    sme_fd_t *fde;
	struct epoll_event events;
    

    e_poll_data_t *ed = mech->priv_data;

    eret = epoll_wait(ed->epoll_fd, &events, 1, tv);

    if (eret == -1) {
        printf("epoll error :%d\n", errno);
        return false;
    }

    if (eret > 0) {
        for (fde = ed->m->fd_events; fde; fde = fde->next) {
            if (events.data.fd == fde->fd) {

                LIST_REMOVE(ed->m->fd_events, fde);
	            epoll_ctl(ed->epoll_fd, EPOLL_CTL_DEL, fde->fd, NULL);
                /*
                 * Trigger the callback
                 */
                fde->cb(ed->m, fde, fde->cb_data);
                break;
            }
        }
    }
    return true;
}

int epoll_mech_loop_wait(sme_mech_t *mech)
{
    bool ret;
    /**
     * Run as much as we have events
     */
    while (sme_loop_has_events(mech)) {
        ret = epoll_mech_loop_once(mech);
        if (ret != true) {
            printf("loop once failed\n");
            return 1;
        }
    }

    printf("No more events. Exiting...\n");
    return 0;
}
