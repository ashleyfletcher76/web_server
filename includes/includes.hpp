#ifndef INCLUDES_HPP
#define INCLUDES_HPP

#define NOSTATUS -5

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <fstream>
#include <sstream>
#include <fcntl.h>
#include <poll.h>
#include <vector>
#include <unordered_map>
#include <map>
#include <limits.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/event.h>
#include <sys/time.h>
#include <exception>
#include <ctime>
#include <iomanip>
#include <arpa/inet.h>
#include <csignal>
#include <set>
#include <netdb.h>
#include <stdexcept>
#include <cctype>

void	log(const std::string& level, const std::string& msg, int client_socket);
extern volatile sig_atomic_t shutdownFlag;
void	signalHandler(int signum);

#endif
