void prefix_line_loop(int sockfd, int fd)
{
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
