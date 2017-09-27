#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>              /* Obtain O_* constant definitions */
#include <unistd.h>
#include "../sme/sme_select.h"
#include "../sme/sme_epoll.h"

void fd_handle(sme_mech_t *m, sme_fd_t *fde, void*data)
{
    char buf[100];
	//float f = 0;
    //read(fde->fd, &f, sizeof(float));
    read(fde->fd, buf, 5);
    printf("Received data : %s\n", buf);
    //printf("Received data : %0.2f\n", f);
}

void pid_handle(sme_mech_t *mech, sme_proc_t *proce, void*data)
{
    printf("child exited..\n");
}

int main()
{
    int pid;// *fd = NULL;
    int p_fd[2];
    int ret;
	int i = 0;
	//char *arg[6];
    sme_mech_t *m = NULL;
    m = epoll_mech_init();
    //create pipe

    //epoll_mech_add_fd(m, p_fd[0], SME_READ, fd_handle, NULL);

	/*arg[0] = "./worker";
	arg[1] = "-x";
	arg[2] = "2";
	arg[3] = "-n";
	arg[4] = "3";
	arg[5] = NULL;*/

	do {
		pipe2(p_fd, O_NONBLOCK);

		pid = fork();
		if(pid < 0) {
			printf("fork failed");
		}
		else if (pid == 0) {
			close(p_fd[0]);
			dup2(p_fd[1], 1);
			//printf("In child writing on fd : %d\n", p_fd[1]);
			//execv(".", arg);
			ret = write(p_fd[1], "hello", 5);
			if (ret < 0)
				write(p_fd[1], "write failed", 10);
			_exit(EXIT_SUCCESS);
		}
		else {
			//printf("In parent\n");
			close(p_fd[1]);
			//select_mech_add_proc(m, pid, 0, pid_handle, fd);
			epoll_mech_add_fd(m, p_fd[0], 0, fd_handle, 0);
		}
		i++;
	}while (i != 5);

    epoll_mech_loop_wait(m);
    return 0;
} 
