void msg_client_left_clr(int sockfd, int fd)
{
	char left[64];

	sprintf(left, "server: client %d just left\n", clients_set[fd].id);
	free(clients_set[fd].in);
	free(clients_set[fd].out);
	clients_set[fd].in = 0;
	clients_set[fd].out = 0;
	clients_set[fd].total_out_sent = 0;
	FD_CLR(fd, &read_fds);
	FD_CLR(fd, &write_fds);
	if (max_fd == fd) {
		while (max_fd > sockfd && !FD_ISSET(max_fd, &read_fds))
			max_fd--;
	}
	msg_to_all(sockfd, fd, left);
	close(fd);
}
