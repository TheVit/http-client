/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2020-01-10 23:45:59
 * @LastEditTime: 2020-05-25 20:56:42
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include <routing.h>
#include "platform_net_socket.h"

int platform_net_socket_connect(const char *host, const char *port, int proto)
{
	int need_record = 0;
	int fd, ret = HTTP_SOCKET_UNKNOWN_HOST_ERROR;
	struct addrinfo hints, *addr_list, *cur;
	char ip[16] = { 0 };
	const char *host_ip = NULL;

	if ((host_ip = routing_search(host)) == NULL) {
		need_record = 1;
		host_ip = host;
	}

	/* Do name resolution with both IPv6 and IPv4 */
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = (proto == PLATFORM_NET_PROTO_UDP) ? SOCK_DGRAM : SOCK_STREAM;
	hints.ai_protocol = (proto == PLATFORM_NET_PROTO_UDP) ? IPPROTO_UDP : IPPROTO_TCP;

	if (getaddrinfo(host_ip, port, &hints, &addr_list) != 0) {
		return ret;
	}

	for (cur = addr_list; cur != NULL; cur = cur->ai_next) {
		fd = socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol);
		if (fd < 0) {
			ret = HTTP_SOCKET_FAILED_ERROR;
			continue;
		}

		if (connect(fd, cur->ai_addr, cur->ai_addrlen) == 0) {
			ret = fd;

			if (need_record == 1) {
				inet_ntop(cur->ai_family, &(((struct sockaddr_in *)(cur->ai_addr))->sin_addr), ip, 16);
				routing_record(host, ip);
			}

			break;
		}

		_close(fd);
		ret = HTTP_CONNECT_FAILED_ERROR;
	}

	freeaddrinfo(addr_list);
	return ret;
}

int platform_net_socket_recv(int fd, void *buf, size_t len, int flags)
{
	return recv(fd, (char*)buf, len, flags);
}

int platform_net_socket_recv_timeout(int fd, unsigned char *buf, int len, int timeout)
{
	int nread;
	int nleft = len;
	char *ptr;
	ptr = (char*)buf;

	struct timeval tv = {
		timeout / 1000,
		(timeout % 1000) * 1000
	};

	if (tv.tv_sec < 0 || (tv.tv_sec == 0 && tv.tv_usec <= 0)) {
		tv.tv_sec = 0;
		tv.tv_usec = 100;
	}

	platform_net_socket_setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv.tv_usec, sizeof(struct timeval));

	while (nleft > 0) {
		nread = platform_net_socket_recv(fd, ptr, nleft, 0);
		if (nread < 0)
		{
			return -1;
		}
		else if (nread == 0) {
			break;
		}

		nleft -= nread;
		ptr += nread;
	}
	return len - nleft;
}


int platform_net_socket_write(int fd, void *buf, size_t len)
{
#ifdef WIN32
	return send(fd, (char*)buf, len, NULL);
#else
	return _write(fd, buf, len);
#endif
}

int platform_net_socket_write_timeout(int fd, unsigned char *buf, int len, int timeout)
{
	struct timeval tv = {
		timeout / 1000,
		(timeout % 1000) * 1000
	};

	if (tv.tv_sec < 0 || (tv.tv_sec == 0 && tv.tv_usec <= 0)) {
		tv.tv_sec = 0;
		tv.tv_usec = 100;
	}

	setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(struct timeval));

#ifdef WIN32
	return send(fd, (char*)buf, len, NULL);
#else
	return _write(fd, buf, len);
#endif
}

int platform_net_socket_close(int fd)
{
#ifdef WIN32
	return closesocket(fd);
#else
	return _close(fd);
#endif
}

int platform_net_socket_set_block(int fd)
{
	// If iMode = 0, blocking is enabled; 
	// If iMode != 0, non-blocking mode is enabled.
	u_long iMode = 0;
	return ioctlsocket(fd, FIONBIO, &iMode);
	//return fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, F_GETFL) & ~O_NONBLOCK);
}

int platform_net_socket_set_nonblock(int fd)
{
	// If iMode = 0, blocking is enabled; 
	// If iMode != 0, non-blocking mode is enabled.
	u_long iMode = 1;
	return ioctlsocket(fd, FIONBIO, &iMode);
	//return fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, F_GETFL) | O_NONBLOCK);
}

int platform_net_socket_setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)
{
	return setsockopt(fd, level, optname, (const char*)optval, optlen);
}

