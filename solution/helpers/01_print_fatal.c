void print_fatal()
{
	write(2, "Fatal error\n", 12);
	exit(1);
}
