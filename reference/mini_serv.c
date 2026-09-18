#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>

int extract_message(char **buf, char **msg)
{
	char	*newbuf;
	int		i;

	*msg = 0;
	if (*buf == 0)
		return (0);
	i = 0;
	while ((*buf)[i])
	{
		if ((*buf)[i] == '\n')
		{
			newbuf = calloc(1,
					sizeof(*newbuf) * (strlen(*buf + i + 1) + 1));
			if (newbuf == 0)
				return (-1);
			strcpy(newbuf, *buf + i + 1);
			*msg = *buf;
			(*msg)[i + 1] = 0;
			*buf = newbuf;
			return (1);
		}
		i++;
	}
	return (0);
}

char *str_join(char *buf, char *add)
{
	char	*newbuf;
	int		len;

	if (buf == 0)
		len = 0;
	else
		len = strlen(buf);
	newbuf = malloc(sizeof(*newbuf) * (len + strlen(add) + 1));
	if (newbuf == 0)
		return (0);
	newbuf[0] = 0;
	if (buf != 0)
		strcat(newbuf, buf);
	free(buf);
	strcat(newbuf, add);
	return (newbuf);
}

typedef struct s_client {
	int id;
	char *in;
	char *out;
	int total_out_sent;
} t_client;

t_client clients_set[FD_SETSIZE];
fd_set read_fds;
fd_set write_fds;
int max_fd;

void print_fatal()
{
	write(2, "Fatal error\n", 12);
	exit(1);
}

void msg_to_all(int sockfd, int skipfd, char *msg)
{
	int fd;

	fd = 0;
	while (fd <= max_fd) {
		if (fd != sockfd && fd != skipfd && FD_ISSET(fd, &read_fds)) {
			clients_set[fd].out = str_join(clients_set[fd].out, msg);
			if (clients_set[fd].out == 0)
				print_fatal();
			FD_SET(fd, &write_fds);
		}
		fd++;
	}
}

void msg_client_arrived(int sockfd, int connfd)
{
	char msg[64];

	sprintf(msg, "server: client %d just arrived\n", clients_set[connfd].id);
	msg_to_all(sockfd, connfd, msg);
}

void prefix_line_loop(int sockfd, int fd) {
	char prefix[64];
	char *line;
	int extract_status;

	sprintf(prefix, "client %d: ", clients_set[fd].id);

	extract_status = extract_message(&clients_set[fd].in, &line);
	while (extract_status == 1) {
		msg_to_all(sockfd, fd, prefix);
		msg_to_all(sockfd, fd, line);
		free(line);
		extract_status = extract_message(&clients_set[fd].in, &line);
	}
	if (extract_status == -1)
		print_fatal();
}


void msg_client_left_clr(int sockfd, int fd)
{
	char msg[64];

	sprintf(msg, "server: client %d just left\n", clients_set[fd].id);
	FD_CLR(fd, &read_fds);
	FD_CLR(fd, &write_fds);
	free(clients_set[fd].in);
	free(clients_set[fd].out);
	clients_set[fd].in = 0;
	clients_set[fd].out = 0;
	clients_set[fd].total_out_sent = 0;
	if (fd == max_fd) {
		while (max_fd > sockfd && !FD_ISSET(max_fd, &read_fds))
			max_fd--;
	}
	msg_to_all(sockfd, fd, msg);
	close(fd);
}

void server_recv_msgs(int sockfd, fd_set ready_read_fds)
{
	int fd;
	char buffer[1025];
	int recv_size;

	fd = 0;
	while (fd <= max_fd) {
		if (fd != sockfd
				&& FD_ISSET(fd, &read_fds)
				&& FD_ISSET(fd, &ready_read_fds)) {
			recv_size = recv(fd, buffer, 1024, 0);
			if (recv_size > 0) {
				buffer[recv_size] = '\0';
				clients_set[fd].in = str_join(clients_set[fd].in, buffer);
				if (clients_set[fd].in == 0)
					print_fatal();
				prefix_line_loop(sockfd, fd);
			}
			else
				msg_client_left_clr(sockfd, fd);
		}
		fd++;
	}
}


void server_send_msgs(int sockfd, fd_set ready_write_fds)
{
	int fd;
	int send_size;

	fd = 0;
	while (fd <= max_fd) {
		if (fd != sockfd
				&& FD_ISSET(fd, &write_fds)
				&& FD_ISSET(fd, &ready_write_fds)) {
			send_size = send(
				fd,
				clients_set[fd].out + clients_set[fd].total_out_sent,
				strlen(clients_set[fd].out) - clients_set[fd].total_out_sent,
				0);
			if (send_size > 0)
				clients_set[fd].total_out_sent += send_size;
			if (clients_set[fd].total_out_sent
				== (int)strlen(clients_set[fd].out)) {
				free(clients_set[fd].out);
				clients_set[fd].out = 0;
				clients_set[fd].total_out_sent = 0;
				FD_CLR(fd, &write_fds);
			}
		}
		fd++;
	}
}

int main(int argc, char **argv)
{
	int sockfd, connfd;
	socklen_t len;
	struct sockaddr_in servaddr, cli;
	fd_set ready_read_fds;
	fd_set ready_write_fds;
	int next_id;

	if (argc != 2) {
		write(2, "Wrong number of arguments\n", 26);
		exit(1);
	}
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
		print_fatal();
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(2130706433);
	servaddr.sin_port = htons(atoi(argv[1]));
	if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
		print_fatal();
	if (listen(sockfd, 10) != 0)
		print_fatal();

	FD_ZERO(&read_fds);
	FD_ZERO(&write_fds);
	FD_SET(sockfd, &read_fds);
	max_fd = sockfd;

	next_id = 0;
	while (1) {
		ready_read_fds = read_fds;
		ready_write_fds = write_fds;
		if (select(max_fd + 1, &ready_read_fds, &ready_write_fds, 0, 0) > 0) {
			if (FD_ISSET(sockfd, &ready_read_fds)) {
				len = sizeof(cli);
				connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
				if (connfd >= 0) {
					clients_set[connfd].id = next_id;
					next_id++;
					clients_set[connfd].in = 0;
					clients_set[connfd].out = 0;
					clients_set[connfd].total_out_sent = 0;
					FD_SET(connfd, &read_fds);
					if (max_fd < connfd)
						max_fd = connfd;
					msg_client_arrived(sockfd, connfd);
				}
			}
			server_recv_msgs(sockfd, ready_read_fds);
			server_send_msgs(sockfd, ready_write_fds);
		}
	}
}
