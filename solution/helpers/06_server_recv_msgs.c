void server_recv_msgs(int sockfd, fd_set ready_read_fds)
{
	int fd;
	int recv_size;
	char buffer[1025];

	fd = 0;
	while (fd <= max_fd) {
		if (fd != sockfd &&
			FD_ISSET(fd, &read_fds) &&
			FD_ISSET(fd, &ready_read_fds)) {
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
