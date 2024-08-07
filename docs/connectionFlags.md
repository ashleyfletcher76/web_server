Overview

The ConnectionHandler module is designed to manage network connections for an HTTP server. It focuses on handling incoming connections efficiently, setting up sockets for non-blocking operation, and integrating with the system's event notification mechanism to manage I/O activities without blocking server operations.
Key Responsibilities

	Accept Connections: Manages the acceptance of new client connections.
	Configure Sockets: Sets sockets to non-blocking mode to improve the server's responsiveness and handling of	simultaneous connections.
	Event Management: Registers sockets with the kqueue system, enabling the server to react to network events (like data availability) efficiently.

'acceptConnection'

This function accepts incoming network connections, sets up the sockets for non-blocking communication, and registers them with kqueue for event notifications.

'configureSocketNonBlocking'
Sets newly accepted sockets to non-blocking mode to ensure that the server does not pause or hang while waiting for I/O operations.

'setupKevent'
Registers sockets with kqueue to notify the server when data is ready to be read or written, enabling efficient data handling.

* Flag Descriptions:
* O_NONBLOCK:
*   - Used to set the file descriptor into non-blocking mode. In this mode,
*     calls like accept, recv, and send do not block and return immediately
*     if they cannot be completed.
*
* EVFILT_READ:
*   - A kevent filter that triggers read events, indicating data is available
*     to be read from the file descriptor.
* EV_ADD:
*   - A kevent flag used to add an event to the kqueue. It registers a new event
*     or modifies an existing one.
* EV_ENABLE:
*   - A kevent flag used to enable an event filter. It is often used in combination
*     with EV_ADD to enable the filter at the time of addition.
* EAGAIN / EWOULDBLOCK:
*   - Errors returned by non-blocking operations when no data is available,
*     indicating the operation would block if the file descriptor was in blocking mode.
*     These errors are checked to handle non-critical situations where an operation
*     cannot immediately proceed.

