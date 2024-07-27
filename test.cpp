#include <vector>
#include <poll.h>
#include <unistd.h>
#include <cstdio>
#include <fcntl.h>

int main()
{
	std::vector<struct pollfd> poll_fds(3);

	poll_fds[0].fd = 0;			 // Monitor stdin (file descriptor 0)
	poll_fds[0].events = POLLIN; // Wait for data to read

	poll_fds[1].fd = open("somefile", O_RDONLY);
	poll_fds[1].events = POLLIN; // Wait for data to read

	poll_fds[2].fd = open("anotherfile", O_RDONLY);
	poll_fds[2].events = POLLIN; // Wait for data to read

	int poll_count = poll(poll_fds.data(), poll_fds.size(), -1);
	if (poll_count == -1)
	{
		perror("poll");
		return 1;
	}

	for (const auto &pfd : poll_fds)
	{
		if (pfd.revents & POLLIN)
		{
			printf("File descriptor %d is ready to read\n", pfd.fd);
			// Read data from pfd.fd
		}
	}

	return 0;
}