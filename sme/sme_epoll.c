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
#define INVALID_MAXFD -1

struct e_poll_data {
    sme_mech_t *m;
    /* epoll create fd */
    int epoll_fd;
	int max_fd;
};

void calc_maxfd_epoll(e_poll_data_t *sa) {

    sme_fd_t *fde;
    sa->max_fd = 0;

    for (fde = sa->m->fd_events; fde; fde = fde->next) {
        if (sa->max_fd < fde->fd)
            sa->max_fd = fde->fd;
    }
}

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
    d->max_fd = INVALID_MAXFD;

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
    if (!fde)
        return NULL;
    
    if ((ed->max_fd != INVALID_MAXFD)
        && (fde->fd > ed->max_fd)) {
        ed->max_fd = fde->fd;
    }

    return fde;
}

bool epoll_mech_loop_once(sme_mech_t *mech)
{
    int sret;
    int isset;//ret = -1;
    sme_fd_t *fde;
    fd_set r_fds, w_fds;
	struct epoll_event event;
	struct epoll_event *events = NULL;
    
    FD_ZERO(&r_fds);
    FD_ZERO(&w_fds);

    e_poll_data_t *sd = mech->priv_data;
    if (sd->max_fd == INVALID_MAXFD) {
        calc_maxfd_epoll(sd);
    }
    for (fde = sd->m->fd_events; fde; fde = fde->next) {
        if (fde->ev == SME_READ) {
            FD_SET(fde->fd, &r_fds);
        }

        if (fde->ev == SME_WRITE) {
            FD_SET(fde->fd, &w_fds);
        }
    }

	/* Buffer where events are returned */
	events = calloc (64, sizeof(event));
	do {
        sret = epoll_wait(sd->epoll_fd, events, 64, -1);
    } while (0 >= sret && EINTR == errno);

    if (sret == -1) {
        printf("epoll error :%d\n", errno);
        return false;
    }
    
    if (sret > 0) {
        /* FD_ISSET(0, &rfds) will be true. */
        isset = 0;
        for (fde = sd->m->fd_events; fde; fde = fde->next) {
            if ((FD_ISSET(fde->fd, &r_fds))
                 && (fde->ev == SME_READ)) {
                isset = 1;
            }

            if ((FD_ISSET(fde->fd, &w_fds))
                 && (fde->ev == SME_WRITE)) {
                isset = 1;;
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
