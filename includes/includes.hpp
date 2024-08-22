#ifndef INCLUDES_HPP
#define INCLUDES_HPP

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
#include <sqlite3.h>
#include <sys/stat.h>
#include <dirent.h>
#include <chrono>

extern volatile sig_atomic_t shutdownFlag;
void	signalHandler(int signum);
bool	checkSocket(int fd);

#endif
