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
