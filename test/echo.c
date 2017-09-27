#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "../sme/sme_select.h"
#include "../sme/sme_epoll.h"

void fd_handle(sme_mech_t *m, sme_fd_t *fde, void*data)
{
	char buf[100];
	read(fde->fd, buf, 5);
	printf("Received data : %s\n", buf);
}

void pid_handle(sme_mech_t *mech, sme_proc_t *proce, void*data)
{
	printf("child exited..\n");
}

int main()
{
	int pid;
	int p_fd[2];
	int ret;
	sme_mech_t *m = NULL;
	m = epoll_mech_init();
	/* create pipe */
	pipe2(p_fd, O_NONBLOCK);

	pid = fork();
	if(pid < 0) {
		printf("fork failed");
	}
	else if (pid == 0) {
		close(p_fd[0]);
		dup2(p_fd[1], 1);
		ret = write(p_fd[1], "hello", 5);
		if (ret < 0)
			write(p_fd[1], "write failed", 10);
		_exit(EXIT_SUCCESS);
	}
	else {
		close(p_fd[1]);
		epoll_mech_add_fd(m, p_fd[0], 0, fd_handle, 0);
	}

	epoll_mech_loop_wait(m);
	return 0;
} 
