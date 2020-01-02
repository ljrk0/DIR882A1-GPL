/* dhcpc.c
 *
 * udhcp DHCP client
 *
 * Russ Dill <Russ.Dill@asu.edu> July 2001
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
 
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/file.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <errno.h>
#include <sys/stat.h>
#include "dhcpd.h"
#include "dhcpc.h"
#include "options.h"
#include "clientpacket.h"
#include "packet.h"
#include "script.h"
#include "socket.h"
#include "debug.h"
#include "pidfile.h"
#include <sys/sysinfo.h>  //brad add 20090314

static int state;
static unsigned long requested_ip; /* = 0 */
//static unsigned long server_addr;
static unsigned long timeout;
static int packet_num; /* = 0 */
static int fd = -1;
static int signal_pipe[2];

#define LISTEN_NONE 0
#define LISTEN_KERNEL 1
#define LISTEN_RAW 2
static int listen_mode;
u_char	svrmac[6];

struct in_addr temp_inet;
 unsigned char *temp;

//brad add 20090314
struct sysinfo system_info;
unsigned long dhcpc_seconds;
//brad add end //brad add 20090314
#define DEFAULT_SCRIPT	"/usr/share/udhcpc/default.script"

struct client_config_t client_config = {
	/* Default options. */
	abort_if_no_lease: 0,
	foreground: 0,
	quit_after_lease: 0,
//	background_if_no_lease: 0,
	interface: "eth1",
	pidfile: NULL,
	script: DEFAULT_SCRIPT,
	clientid: NULL,
	hostname: NULL,
	ifindex: 0,
	arp: "\0\0\0\0\0\0",		/* appease gcc-3.0 */


};


/* just a little helper */
static void change_mode(int new_mode)
{
	//printf("-----change_mode------\n");
	DEBUG(LOG_INFO, "entering %s listen mode",
		new_mode ? (new_mode == 1 ? "kernel" : "raw") : "none");
	if (fd >= 0) close(fd);
	fd = -1;
	listen_mode = new_mode;
}



/* Exit and cleanup */
static void exit_client(int retval)
{
	//printf("---exit client------\n");
	pidfile_delete(client_config.pidfile);
	CLOSE_LOG();
	exit(retval);
}

/* Signal handler */
static void signal_handler(int sig)
{
	printf("---signal_handler--\n");
	if (send(signal_pipe[1], &sig, sizeof(sig), MSG_DONTWAIT) < 0) {
		LOG(LOG_ERR, "Could not send signal: %s",
			strerror(errno));
	}
}

int main(int argc, char *argv[])
{
	printf("---udhcpc-discovery start-------\n");
	unsigned char *temp, *message;
	//unsigned long t1 = 0, t2 = 0, xid = 0;
	//unsigned long start = 0, lease;
	unsigned long xid = 0;
	fd_set rfds;
	int retval;
	struct timeval tv;
	int c, len;
	struct dhcpMessage packet;
	//struct in_addr temp_addr;
	int pid_fd;
	time_t now;
	int max_fd;
	//int sig;
	unsigned char *server_id;
	u_int32_t server_id_align;

	static struct option arg_options[] = {
		{"clientid",	required_argument,	0, 'c'},
		{"foreground",	no_argument,		0, 'f'},
		{"background",	no_argument,		0, 'b'},
		{"hostname",	required_argument,	0, 'H'},
		{"hostname",    required_argument,      0, 'h'},
		{"interface",	required_argument,	0, 'i'},
		{"now", 	no_argument,		0, 'n'},
		{"pidfile",	required_argument,	0, 'p'},
		{"quit",	no_argument,		0, 'q'},
		{"request",	required_argument,	0, 'r'},
		{"script",	required_argument,	0, 's'},

		{"version",	no_argument,		0, 'v'},
		{"help",	no_argument,		0, '?'},
		{0, 0, 0, 0}
	};

	/* get options */
	while (1) {
		int option_index = 0;
		c = getopt_long(argc, argv, "c:fbH:h:i:np:qr:s:v", arg_options, &option_index);
		//printf("--c:%d---\n",c);
		//printf("--c:%c---\n",c);
		if (c == -1) break;		
		switch (c) {
			//printf("--c:%d---\n",c);
		case 'i':
			client_config.interface =  optarg;
			break;
		case '?':
			printf("---------udhcpc-discovery HELP-----------\n");
			printf("Cmd format:udhcpc-discovery -i eth2.2 \n");
			printf("Get help:udhcpc-discovery -h \n");
			break;	
		default:
			printf("---------udhcpc-discovery HELP-----------\n");
			printf("Cmd format:udhcpc-discovery -i eth2.2 \n");
			printf("Get help:udhcpc-discovery -h \n");
		}
	}

	OPEN_LOG("udhcpc");
	LOG(LOG_INFO, "udhcpc-discovery client (v%s) started", VERSION);

	pid_fd = pidfile_acquire(client_config.pidfile);
	pidfile_write_release(pid_fd);

	if (read_interface(client_config.interface, &client_config.ifindex, 
			   NULL, client_config.arp) < 0)
		exit_client(-1);
		
	if (!client_config.clientid) {
		client_config.clientid = malloc(6 + 3);
		client_config.clientid[OPT_CODE] = DHCP_CLIENT_ID;
		client_config.clientid[OPT_LEN] = 7;
		client_config.clientid[OPT_DATA] = 1;
		memcpy(client_config.clientid + 3, client_config.arp, 6);
	}

	/* setup signal handlers */
	socketpair(AF_UNIX, SOCK_STREAM, 0, signal_pipe);
	signal(SIGUSR1, signal_handler);
	signal(SIGUSR2, signal_handler);
	signal(SIGTERM, signal_handler);
	
	state = INIT_SELECTING;
	run_script(NULL, "deconfig");
	change_mode(LISTEN_RAW);
	//brad add 20090314
	sysinfo(&system_info);
	dhcpc_seconds = (unsigned long) system_info.uptime;
	//end of brad add 20090314
	for (;;) {
	
		tv.tv_sec = timeout - dhcpc_seconds; //brad add 20090314
		
		tv.tv_usec = 0;
		FD_ZERO(&rfds);

		if (listen_mode != LISTEN_NONE && fd < 0) {
			if (listen_mode == LISTEN_KERNEL)
				fd = listen_socket(INADDR_ANY, CLIENT_PORT, client_config.interface);
			else
				fd = raw_socket(client_config.ifindex);
			if (fd < 0) {
				LOG(LOG_ERR, "FATAL: couldn't listen on socket, %s", strerror(errno));
				exit_client(-1);
			}
		}
		if (fd >= 0) FD_SET(fd, &rfds);
		FD_SET(signal_pipe[0], &rfds);		

		if (tv.tv_sec > 0) {
			DEBUG(LOG_INFO, "Waiting on select...\n");
			max_fd = signal_pipe[0] > fd ? signal_pipe[0] : fd;
			retval = select(max_fd + 1, &rfds, NULL, NULL, &tv);
			//printf("--retval-------------%d--------\n",retval);
		} else retval = 0; /* If we already timed out, fall through */

		//now = time(0); //brad modify, we do not care system time 20090314
		//brad add 20090314
		sysinfo(&system_info);
		dhcpc_seconds = (unsigned long) system_info.uptime;
		now = dhcpc_seconds;
		//end of brad add 20090314
		if (retval == 0) {
			/* timeout dropped to zero */
			if (state==INIT_SELECTING) {
				//printf("-----INIT_SELECTING----------\n");
				if(packet_num ==3)
					{
						//system("nvram_set 2860 udhcpcdetect noudhcpc");
						//system("echo udhcpc-discovery=fail > /etc/udhcpcdetect");
						system("nvram_set 2860 wan_wan0_dhcp_detect 0");
						//system("rm -rf /var/DhcpDetect");
						system("echo == dhcp fail==");
						exit_client(-1);
					}
				if (packet_num < 3) {
					//printf("------packet_num---%d-------------",packet_num);
					if (packet_num == 0)
						xid = random_xid();

					/* send discover packet */
					send_discover_dis(xid, requested_ip); /* broadcast */
					timeout = now + ((packet_num == 2) ? 4 : 2);
					packet_num++;
				} 
				//break;

			}
		}
		else if (retval > 0 && listen_mode != LISTEN_NONE && FD_ISSET(fd, &rfds)) {
			//printf("-----------else---------------------\n");
			/* a packet is ready, read it */
			
			if (listen_mode == LISTEN_KERNEL)
				len = get_packet(&packet, fd);
			else len = get_raw_packet(&packet, fd);
						
			if ((message = get_option(&packet, DHCP_MESSAGE_TYPE)) == NULL) {
				//printf("---------message error--------\n");
				continue;
			}
			
			server_id = get_option(&packet, DHCP_SERVER_ID);
			if (server_id) memcpy(&server_id_align, server_id, 4);
			DEBUG(LOG_WARNING, "===the wan dhcp server is %d===\n",server_id_align,server_id);

               	if (!(temp = get_option(&packet, DHCP_SUBNET)))
                        memset(&temp_inet.s_addr, 0x00, 4);
                    else
                        memcpy(&temp_inet.s_addr, temp, 4);
			//LOG(LOG_INFO, "===the wan dhcp server mask is %lu===\n", temp_inet);
			
			if(state==INIT_SELECTING) {
				/* Must be a DHCPOFFER to one of our xid's */
				if (*message == DHCPOFFER) {
					//printf("---------DHCPOFFER--------\n");
					//system("echo udhcpc-discovery=success > /etc/udhcpcdetect");
					system("echo == dhcp success ==");
					system("touch /var/DhcpDetect");
					exit_client(0);
				}
			}	
		} 
			
	}
	return -1;
}