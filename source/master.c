#define _GNU_SOURCE
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <getopt.h>
#include "../sme/sme_select.h"
#include "../sme/sme_epoll.h"

#define SUCCESS 0
#define FAILURE 1

float g_summation = 0;
char g_worker_path[256] = {'\0'};
int g_num_workers = 0;
int g_active_workers = 0;
int g_current_n = 1;
int g_in_x = 0;
int g_in_n = 0;

typedef struct wait_mech
{
	char mechanism_name[11];
	int mechanism_id;
}wait_mech;

void pid_handle(sme_mech_t *mech, sme_proc_t *proce, void *data);
void fd_handle(sme_mech_t *mech, sme_fd_t *fde, void *data);
void fd_handle_epoll(sme_mech_t *mech, sme_fd_t *fde, void *data);

wait_mech valid_wait_mech[4] = {{"sequential", 0},
								{"select", 1},
								{"poll", 2},
								{"epoll", 3}};

void str_reverse(char *str) {
	int var = 0, i = 0, j = 0;	
	j = strlen(str) - 1;

	while (i < j) {
		var = str[i];
		str[i] = str[j];
		str[j] = var;
		i++;
		j--;
	}
}

void integer_to_string(int number, char *str) {
	int rem = -1, i = 0;

	while (number != 0) {
		rem = number % 10;
		number = number / 10;
		str[i] = rem + 48;
		i++;
	}
	str_reverse(str);
}

bool validate_and_get_args(int argc,
		char **argv,
		char *worker_path,
		char *wait_mechanism,
		int *num_workers,
		int *x,
		int *n)
{
	int ch;

	if (argc == 1) {
		printf("Supply arguments with -x and -n\n");
		exit(1);
	}

	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{"worker_path",    required_argument, 0, 'a'},
			{"num_workers",    required_argument, 0, 'b'},
			{"wait_mechanism", required_argument, 0, 'c'},
			{"x", 			   required_argument, 0, 'x'},
			{"n",  			   required_argument, 0, 'n'},
			{0, 			   0, 			      0, 0}};

		ch = getopt_long(argc, argv, "a:b:c:x:n:",
				long_options, &option_index);
		if (ch == -1)
			break;

		switch(ch) {	
			case 'a':
				strcpy(worker_path, optarg);
				strcpy(g_worker_path, worker_path);
				break;
			case 'b':
				*num_workers = atoi(optarg);
				g_num_workers = *num_workers;
				break;
			case 'c':
				strcpy(wait_mechanism, optarg);
				break;
			case 'x':
				*x = atoi(optarg);
				g_in_x = *x;
				break;
			case 'n':
				*n = atoi(optarg);
				g_in_n = *n;
				break;
			default:
				printf("Invalid arguments\n");
				return false;
		}
	}

	if ((*n < 0)
			|| (*num_workers <= 0)
			|| (strcmp(worker_path, "") == 0)
			|| (strcmp(wait_mechanism, "") == 0)) {
		printf("Invalid arguments\n");
		printf("Usage ./master --worker_path ./worker "
				"--num_workers 5 --wait_mechanism MECHANISM "
				"-x 2 -n 12\n");
		return false;
	}

	return true;
}

int process_command(const char *worker_path,
		int x,
		int n,
		int pipe_fd[2])
{
	int ret = -1;
	char *arg[6] = {0};
	char x_str[10] = {'\0'}, n_str[10] = {'\0'};

	/* Close the read end of the unnamed pipe */
	ret = close(pipe_fd[0]);
	if (ret == -1) {
		printf("Unable to close read end of pipe : %d\n", errno);
		ret = FAILURE;
		goto error_out;
	}

	/**
	 * Duplicate the file descriptors of the standard output as to
	 * get the output in the unnamed pipe
	 */	
	ret = dup2(pipe_fd[1], 1); /* duplicate the std output */
	if (ret == -1) {
		printf("Unable to duplicate stdout FD : %d\n", errno);
		ret = FAILURE;
		goto error_out;
	}

	integer_to_string(x, x_str);
	integer_to_string(n, n_str);
	arg[0] = ".";
	arg[1] = "-x";
	arg[2] = x_str;
	arg[3] = "-n";
	arg[4] = n_str;
	arg[5] = NULL;

	ret = execv(worker_path, arg);
	if (ret == -1) {
		printf("Unable to create new worker thread  : %d\n", errno);
		ret = FAILURE;
		goto error_out;
	}

	exit(0);

error_out:
	return ret;
}

typedef struct seq_cb_data {
	char *worker_path;
	int fd;
	int x;
	int n;
} seq_cb_data_t;

int exec_worker_spawn(sme_mech_t *m,
		const char *worker_path,
		int x,
		int n,
		int flag)
{
	int pipe_fd[2];
	int pid = 0, ret = -1;

	/* create pipe */
	ret = pipe2(pipe_fd, O_NONBLOCK);
	if (ret == -1) {
		printf("Unable to create an unnamed pipe :%d\n", errno);
		goto error_out;
	}

	pid = fork();
	if (pid < 0) {
		printf("Unable to fork a child process :%d\n", errno);
		goto error_out;
	}
	else if (pid == 0) {
		/* In child */
		ret = process_command(worker_path, x, n, pipe_fd);
		if (ret == FAILURE) {
			printf("Unable to process command\n");
			goto error_out;
		}
	}
	else
	{
		/* In parent */
		g_active_workers++;
		g_current_n++;

		close(pipe_fd[1]);

		/* Sequential mechanism selected */
		if (flag == 0) {
			int *fd  = calloc(1, sizeof(int));
			*fd = pipe_fd[0];
			select_mech_add_proc(m, pid, 0, pid_handle, fd);
		}
		else if(flag == 1) {
			/* Select mechanism selected */
			select_mech_add_fd(m, pipe_fd[0], 0, fd_handle, 0);
		}
		else if (flag == 2) {
			/* Epoll mechanism selected */
			epoll_mech_add_fd(m, pipe_fd[0], 0, fd_handle_epoll, 0);
		}
	}

error_out:
	return 0;
}

void pid_handle(sme_mech_t *mech, sme_proc_t *proce, void *data)
{
	float worker_data;
	int *fd  = (int*) data;

	read(*fd, &worker_data, sizeof(worker_data));

	printf("Received data : %0.9f\n", worker_data);
	g_summation += worker_data;

	g_active_workers--;

	if ((g_active_workers < g_num_workers)
			&& (g_current_n <= g_in_n)) {

		exec_worker_spawn(mech,
				g_worker_path,
				g_in_x, g_current_n, 0);
	}
}

void fd_handle(sme_mech_t *m, sme_fd_t *fde, void *data)
{
	float worker_data = 0;

	read(fde->fd, &worker_data, sizeof(worker_data));
	printf("Received data : %0.9f\n", worker_data);
	g_summation += worker_data;

	g_active_workers--;

	if ((g_active_workers < g_num_workers)
			&& (g_current_n <= g_in_n)) {

		exec_worker_spawn(m, g_worker_path,
				g_in_x, g_current_n, 1);
	}
}

void fd_handle_epoll(sme_mech_t *m, sme_fd_t *fde, void *data)
{
	float worker_data = 0;

	read(fde->fd, &worker_data, sizeof(worker_data));
	printf("Received data : %0.9f\n", worker_data);
	g_summation += worker_data;

	g_active_workers--;

	if ((g_active_workers < g_num_workers)
			&& (g_current_n <= g_in_n)) {

		exec_worker_spawn(m, g_worker_path,
				g_in_x, g_current_n, 2);
	}
}

float epoll_mech(char *worker_path,
		int num_workers,
		int x,
		int n)
{
	int k = 1;
	sme_mech_t *m = NULL;
	m = epoll_mech_init();

	/* Launch initial num of workers */
	do {
		/* Implemented epoll mechanism by monitoring fd's */
		exec_worker_spawn(m, g_worker_path,
				x, k, 2);
		k++;
	} while((k <= n) && (k <= num_workers));

	epoll_mech_loop_wait(m);

	g_summation += 1;

	return 0;
}

float select_mech(char *worker_path,
		int num_workers,
		int x,
		int n)
{
	int k = 1;
	sme_mech_t *m = NULL;
	m = select_mech_init();

	/* Launch initial num of workers */
	do {
		/* Implemented select mechanism by monitoring fd's */
		exec_worker_spawn(m, g_worker_path,
				x, k, 1);
		k++;
	} while((k <= n) && (k <= num_workers));

	select_mech_loop_wait(m);

	g_summation += 1;

	return 0;
}

float sequential_mech(char *worker_path,
		int num_workers,
		int x,
		int n)
{
	int k = 1;
	sme_mech_t *m = NULL;
	m = select_mech_init();

	/* Launch initial num of workers */
	do {
		/**
		 * Implemented sequential using select mechanism itself by
		 * monitoring pid instead of fd's
		 */
		exec_worker_spawn(m, g_worker_path,
				x, k, 0);
		k++;
	} while((k <= n) && (k <= num_workers));

	select_mech_loop_wait(m);

	g_summation += 1;

	return 0;
}

int main(int argc, char** argv) {
	bool ret = false;
	int x = 0, n = 0, mech_id = 0, no = 0, num_workers = 0;
	char worker_path[256] = {'\0'};
	char wait_mechanism[12] = {'\0'};

	ret = validate_and_get_args(argc, argv,
			worker_path,
			wait_mechanism,
			&num_workers,
			&x, &n);
	if (ret == false) {
		printf("Argument parsing failed\n");
		goto error_out;
	}

	while (no != 4) {
		if(strcmp(wait_mechanism,
					valid_wait_mech[no].mechanism_name) == 0) {
			mech_id = valid_wait_mech[no].mechanism_id;
			printf("Mechanism selected : %s\n",
					valid_wait_mech[no].mechanism_name);
			break;
		}
		no++;
	}

	switch(mech_id) {
		case 0:
			sequential_mech(worker_path,
					num_workers,
					x, n);
			break;
		case 1:
			select_mech(worker_path,
					num_workers,
					x, n);
			break;
		case 3:
			epoll_mech(worker_path,
					num_workers,
					x, n);
			break;

		default:
			printf("Unsupported mechanism\n");

	}

	printf("Summation is : %0.9f\n", g_summation);

	return SUCCESS;

error_out:
	return FAILURE;
}
