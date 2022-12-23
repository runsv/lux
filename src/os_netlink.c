/*
 * netlink uevents
 */

#if 0
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/connector.h>
#define _LINUX_TIME_H
#include <linux/cn_proc.h>
#include <errno.h>

/* socket for netlink connection  */
static int nl_sock ;

#define NL_MESSAGE_SIZE (sizeof(struct nlmsghdr) + sizeof(struct cn_msg) + \
                         sizeof(int))

void connect_to_netlink ( void )
{
  struct sockaddr_nl sa_nl; /* netlink interface info */
  char buff[NL_MESSAGE_SIZE];
  struct nlmsghdr *hdr; /* for telling netlink what we want */
  struct cn_msg *msg;   /* the actual connector message */

  /* connect to netlink socket */
  nl_sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_CONNECTOR);

  if (-1 == nl_sock) {
    rb_raise(rb_eStandardError, "%s", strerror(errno));
  }

  bzero(&sa_nl, sizeof(sa_nl));
  sa_nl.nl_family = AF_NETLINK;
  sa_nl.nl_groups = CN_IDX_PROC;
  //sa_nl.nl_pid = getpid();
  sa_nl.nl_pid = 0 ;

  if ( 0 > bind(nl_sock, (struct sockaddr *)&sa_nl, sizeof(sa_nl))) {
    rb_raise(rb_eStandardError, "%s", strerror(errno));
  }

  /* Fill header */
  hdr = (struct nlmsghdr *)buff;
  hdr->nlmsg_len = NL_MESSAGE_SIZE;
  hdr->nlmsg_type = NLMSG_DONE;
  hdr->nlmsg_flags = 0;
  hdr->nlmsg_seq = 0;
  hdr->nlmsg_pid = getpid();

  /* Fill message */
  msg = (struct cn_msg *)NLMSG_DATA(hdr);
  msg->id.idx = CN_IDX_PROC;  /* Connecting to process information */
  msg->id.val = CN_VAL_PROC;
  msg->seq = 0;
  msg->ack = 0;
  msg->flags = 0;
  msg->len = sizeof(int);
  * (int *) msg->data = PROC_CN_MCAST_LISTEN;

  if (-1 == send ( nl_sock, hdr, hdr -> nlmsg_len, 0 ) ) {
    rb_raise ( rb_eStandardError, "%s", strerror ( errno ) ) ;
  }
}

/* end of Ruby Ext */

#ifndef SO_RCVBUFFORCE
#define SO_RCVBUFFORCE 33
#endif
enum { RCVBUF = 2 * 1024 * 1024 } ;

int uevent_main ( int argc, char ** argv )
{
  int fd ;
  struct sockaddr_nl sa ;

  INIT_G() ;
  argv ++ ;

  // Subscribe for UEVENT kernel messages
  sa.nl_family = AF_NETLINK;
  sa.nl_pad = 0;
  sa.nl_pid = getpid();
  sa.nl_groups = 1 << 0;
  fd = xsocket(AF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
  xbind ( fd, (struct sockaddr *) &sa, sizeof(sa) ) ;
  close_on_exec_on ( fd ) ;

	// Without a sufficiently big RCVBUF, a ton of simultaneous events
	// can trigger ENOBUFS on read, which is unrecoverable.
	// Reproducer:
	//	uevent mdev &
	// 	find /sys -name uevent -exec sh -c 'echo add >"{}"' ';'
	//
	// SO_RCVBUFFORCE (root only) can go above net.core.rmem_max sysctl
  setsockopt_SOL_SOCKET_int ( fd, SO_RCVBUF,      RCVBUF ) ;
  setsockopt_SOL_SOCKET_int ( fd, SO_RCVBUFFORCE, RCVBUF ) ;

  if ( 0 ) {
    int z ;
    socklen_t zl = sizeof ( z ) ;
    getsockopt ( fd, SOL_SOCKET, SO_RCVBUF, &z, &zl ) ;
    bb_error_msg ( "SO_RCVBUF:%d", z ) ;
  }

  while ( 1 ) {
    int idx ;
    ssize_t len ;
    char * netbuf ;
    char * s, * end ;

		// In many cases, a system sits for *days* waiting
		// for a new uevent notification to come in.
		// We use a fresh mmap so that buffer is not allocated
		// until kernel actually starts filling it.
		netbuf = mmap(NULL, BUFFER_SIZE,
					PROT_READ | PROT_WRITE,
					MAP_PRIVATE | MAP_ANON,
					/* ignored: */ -1, 0);
		if (netbuf == MAP_FAILED)
			bb_perror_msg_and_die("mmap");

		// Here we block, possibly for a very long time
		len = safe_read(fd, netbuf, BUFFER_SIZE - 1);
		if (len < 0)
			bb_perror_msg_and_die("read");
		end = netbuf + len;
		*end = '\0';

		// Each netlink message starts with "ACTION@/path"
		// (which we currently ignore),
		// followed by environment variables.
		if (!argv[0])
			putchar('\n');
		idx = 0;
		s = netbuf;
		while (s < end) {
			if ( ! argv [ 0 ] )
				puts ( s );
			if ( strchr ( s, '=' ) && idx < MAX_ENV )
				env [ idx ++ ] = s ;
			s += strlen(s) + 1;
		}

		env [ idx ] = NULL ;
		idx = 0 ;
		while ( env [ idx ] )
			putenv ( env [ idx ++ ] ) ;

		if ( argv [ 0 ] )
			spawn_and_wait ( argv ) ;
		idx = 0 ;
		while ( env [ idx ] )
			bb_unsetenv ( env [ idx ++ ] ) ;
		munmap ( netbuf, BUFFER_SIZE ) ;
  }

  return 0 ;
}

/*
 * Arachsys uevent(d)
 */

#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <sys/socket.h>

void error(int status, int errnum, char *format, ...) {
  va_list args;

  fprintf(stderr, "%s: ", progname);
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  if (errnum != 0)
    fprintf(stderr, ": %s\n", strerror(errnum));
  else
    fputc('\n', stderr);
  if (status != 0)
    exit(status);
}

/* Pass on HUP, INT, TERM, USR1, USR2 signals, and exit on SIGTERM. */
int uevent_main ( int argc, char ** argv )
{
  char buffer[BUFFER + 1], *cursor, *separator;
  int sock;
  ssize_t length;
  struct sockaddr_nl addr;

  if (argc > 1)
    subprocess(argv + 1);

  memset(&addr, 0, sizeof(addr));
  addr.nl_family = AF_NETLINK;
  addr.nl_pid = getpid();
  addr.nl_groups = 1;

  if ((sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_)) < 0)
    error(EXIT_FAILURE, errno, "socket");

  if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0)
    error(EXIT_FAILURE, errno, "bind");

  while ( 1 ) {
    if ((length = recv(sock, &buffer, sizeof(buffer) - 1, 0)) < 0) {
      if (errno != EAGAIN && errno != EINTR)
        error(EXIT_FAILURE, errno, "recv");
      continue;
    }

    /* Null-terminate the uevent and replace stray newlines with spaces. */
    buffer[length] = 0;
    for (cursor = buffer; cursor < buffer + length; cursor++)
      if (*cursor == '\n')
        *cursor = ' ';

    if (strlen(buffer) >= length - 1) {
      /* No properties; fake a simple environment based on the header. */
      if ((cursor = strchr(buffer, '@'))) {
        *cursor++ = 0;
        printf("ACTION %s\n", buffer);
        printf("DEVPATH %s\n", cursor);
      }
    } else {
      /* Ignore header as properties will include ACTION and DEVPATH. */
      cursor = buffer;
      while (cursor += strlen(cursor) + 1, cursor < buffer + length) {
        if ((separator = strchr(cursor, '=')))
          *separator = ' ';
        puts(cursor);
      }
    }
    putchar('\n');
    fflush(stdout);
  }

  return EXIT_FAILURE;
}

/*
 * Netlink functions for IFUP/IFDN/GW events
 */

#include <arpa/inet.h>
#include <errno.h>
#include <net/if.h>		/* IFNAMSIZ */
#include <sys/socket.h>
#include <linux/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <unistd.h>

static int nlmsg_validate ( struct nlmsghdr * nh, size_t len )
{
	if (!NLMSG_OK(nh, len))
		return 1;

	if (nh->nlmsg_type == NLMSG_DONE) {
		_d("Done with netlink messages.");
		return 1;
	}

	if (nh->nlmsg_type == NLMSG_ERROR) {
		_d("Netlink reports error.");
		return 1;
	}

	return 0;
}

static void nl_route ( struct nlmsghdr * nlmsg )
{
	struct rtmsg *r;
	struct rtattr *a;
	int la;
	int gw = 0, dst = 0, mask = 0, idx = 0;

	if ( nlmsg -> nlmsg_len < NLMSG_LENGTH( sizeof ( struct rtmsg ) ) ) {
		_e("Packet too small or truncated!");
		return;
	}

	r  = NLMSG_DATA(nlmsg);
	a  = RTM_RTA(r);
	la = RTM_PAYLOAD(nlmsg);
	while (RTA_OK(a, la)) {
		void *data = RTA_DATA(a);
		switch (a->rta_type) {
		case RTA_GATEWAY:
			gw = *((int *)data);
			//_d("GW: 0x%04x", gw);
			break;

		case RTA_DST:
			dst = *((int *)data);
			mask = r->rtm_dst_len;
			//_d("MASK: 0x%04x", mask);
			break;

		case RTA_OIF:
			idx = *((int *)data);
			//_d("IDX: 0x%04x", idx);
			break;
		}

		a = RTA_NEXT(a, la);
	}

	if ((!dst && !mask) && (gw || idx)) {
		if (nlmsg->nlmsg_type == RTM_DELROUTE)
			cond_clear("net/route/default");
		else
			cond_set("net/route/default");
	}
}

static void nl_link(struct nlmsghdr *nlmsg)
{
	int la;
	char ifname[IFNAMSIZ + 1];
	struct rtattr *a;
	struct ifinfomsg *i;

	if (nlmsg->nlmsg_len < NLMSG_LENGTH(sizeof(struct ifinfomsg))) {
		_e("Packet too small or truncated!");
		return;
	}

	i  = NLMSG_DATA(nlmsg);
	a  = (struct rtattr *)((char *)i + NLMSG_ALIGN(sizeof(struct ifinfomsg)));
	la = NLMSG_PAYLOAD(nlmsg, sizeof(struct ifinfomsg));

	while (RTA_OK(a, la)) {
		if (a->rta_type == IFLA_IFNAME) {
			char msg[MAX_ARG_LEN];

			strlcpy(ifname, RTA_DATA(a), sizeof(ifname));
			switch (nlmsg->nlmsg_type) {
			case RTM_NEWLINK:
				/*
				 * New interface has appearad, or interface flags has changed.
				 * Check ifi_flags here to see if the interface is UP/DOWN
				 */
				if (i->ifi_change & IFF_UP) {
					snprintf(msg, sizeof(msg), "net/%s/up", ifname);

					if (i->ifi_flags & IFF_UP)
						cond_set(msg);
					else
						cond_clear(msg);

					if (string_compare("lo", ifname)) {
						snprintf(msg, sizeof(msg), "net/%s/exist", ifname);
						cond_set(msg);
					}
				} else {
					snprintf(msg, sizeof(msg), "net/%s/exist", ifname);
					cond_set(msg);
				}
				break;

			case RTM_DELLINK:
				/* NOTE: Interface has dissapeared, not link down ... */
				snprintf(msg, sizeof(msg), "net/%s/exist", ifname);
				cond_clear(msg);
				break;

			case RTM_NEWADDR:
				_d("%s: New Address", ifname);
				break;

			case RTM_DELADDR:
				_d("%s: Deconfig Address", ifname);
				break;

			default:
				_d("%s: Msg 0x%x", ifname, nlmsg->nlmsg_type);
				break;
			}
		}

		a = RTA_NEXT(a, la);
	}
}

static void nl_callback(void *UNUSED(arg), int sd, int UNUSED(events))
{
	ssize_t len;
	static char buf[4096];
	struct nlmsghdr *nh;

	memset(buf, 0, sizeof(buf));
	len = recv(sd, buf, sizeof(buf), 0);
	if (len < 0) {
		if (errno != EINTR)	/* Signal */
			_pe("recv()");
		return;
	}

	for (nh = (struct nlmsghdr *)buf; !nlmsg_validate(nh, len); nh = NLMSG_NEXT(nh, len)) {
		//_d("Well formed netlink message received. type %d ...", nh->nlmsg_type);
		if (nh->nlmsg_type == RTM_NEWROUTE || nh->nlmsg_type == RTM_DELROUTE)
			nl_route(nh);
		else
			nl_link(nh);
	}
}

PLUGIN_INIT( plugin_init )
{
	int sd;
	struct sockaddr_nl sa;

	sd = socket(AF_NETLINK, SOCK_RAW | SOCK_NONBLOCK | SOCK_CLOEXEC, NETLINK_ROUTE);
	if (sd < 0) {
		_pe("socket()");
		return;
	}

	memset(&sa, 0, sizeof(sa));
	sa.nl_family = AF_NETLINK;
	sa.nl_groups = RTMGRP_IPV4_ROUTE | RTMGRP_LINK; // | RTMGRP_NOTIFY | RTMGRP_IPV4_IFADDR;
	sa.nl_pid    = getpid();

	if (bind(sd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
		_pe("bind()");
		close(sd);
		return;
	}

  plugin.io.fd = sd ;
  plugin_register ( & plugin ) ;
}

#endif /* #if 0 */
