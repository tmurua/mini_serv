void server_send_msgs(int sockfd, fd_set ready_write_fds)
{
	int fd;
	int send_size;

	fd = 0;
	while (fd <= max_fd) {
		if (fd != sockfd &&
			FD_ISSET(fd, &write_fds) &&
			FD_ISSET(fd, &ready_write_fds)) {
			send_size = send(
				fd,
				clients_set[fd].out + clients_set[fd].total_out_sent,
				strlen(clients_set[fd].out) - clients_set[fd].total_out_sent,
				MSG_NOSIGNAL
			);
			if (send_size > 0)
				clients_set[fd].total_out_sent += send_size;
			if ((int)strlen(clients_set[fd].out)
				== clients_set[fd].total_out_sent) {
				free(clients_set[fd].out);
				clients_set[fd].out = 0;
				clients_set[fd].total_out_sent = 0;
				FD_CLR(fd, &write_fds);
			}
		}
		fd++;
	}
}
