void msg_client_arrived(int sockfd, int connfd)
{
	char msg[64];

	sprintf(msg, "server: client %d just arrived\n", clients_set[connfd].id);
	msg_to_all(sockfd, connfd, msg);
}
