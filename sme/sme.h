#ifndef __SME_H_
#define __SME_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct sme_mech sme_mech_t;
typedef struct sme_ops sme_ops_t;
typedef struct sme_fd sme_fd_t;
typedef struct sme_proc sme_proc_t;
typedef enum  fd_event fd_event_t;


/**
 * callbacks for event
 */

/*
 * fd event callback
 */
typedef void (*sme_fd_cb)(sme_mech_t *mech,
                          sme_fd_t *fde,
                          void *data); 
/*
 * process event callback
 */
typedef void (*sme_proc_cb)(sme_mech_t *mech,
                          sme_proc_t *pe,
                          void *data); 

/*
 * Add a fd event
 */
sme_fd_t *sme_comm_add_fd(sme_mech_t *mech,
                     int fd,
                     fd_event_t ev,
                     sme_fd_cb cb,
                     void *cb_data);

sme_proc_t *sme_comm_add_proc(sme_mech_t *mech,
                          int pid,
                          int flags,
                          sme_proc_cb cb,
                          void *cb_data);

int sme_comm_loop_once(sme_mech_t *mech);
int sme_comm_loop_wait(sme_mech_t *mech);
bool sme_comm_loop_proc(sme_mech_t *mech);
bool sme_loop_has_events(sme_mech_t *mech);
#endif
