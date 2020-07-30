#include <shout/shout.h>
#include <stdio.h>
#include <unistd.h>

#define BUFSIZE 4096
#define VERSION "1.02"

void usage() {
	fprintf(stderr, "Usage: shoutsend [options ...]\n"
	"Version " VERSION "\n"
	"Options:\n"
	"\t-s host\t\tserver (default: 127.0.0.1)\n"
	"\t-p port\t\tport (default: 8000)\n"
	"\t-U user\t\tusername (default: source)\n"
	"\t-P pass\t\tpassword (default: letmein)\n"
	"\t-o\t\tset data format to OGG/Vorbis (default: MP3)\n"
	"\t-m mnt\t\tmountpoint (default: /live.mp3)\n"
	"\t-d description\tstream description (default: none)\n"
	"\t-u url\t\tstream description url (default: none)\n"
	"\t-1\t\tuse AudioCast protocol (IceCast1 compatible)\n"
	"\t-S\t\tuse ShoutCast protocol (ShoutCast compatible)\n"
	"\t\t\tdefault protocol: HTTP (IceCast2 compatible)\n");
}

int main (int argc, char **argv) {
	char *ip = "127.0.0.1";
	int port = 8000;
	char *password = "letmein";
	char *mountpoint = "/live";
	char *nazwa = "";
	char *opis = "";
	char *url = "";
	char *user = "source";
	int total, read, ret, c;
	int format = SHOUT_FORMAT_MP3;
	int protocol = SHOUT_PROTOCOL_HTTP;
	char buff[BUFSIZE+2];
	shout_t *conn;

	while ((c = getopt(argc, argv, "hs:p:P:m:n:d:u:U:o1S")) != -1) {
		int i;
		switch(c) {
		case 'h':
			usage();
			return 0;
		case 's':
			ip = optarg;
			break;
		case 'p':
			port = atoi(optarg);
			break;
		case 'P':
			password = (char *)malloc(strlen(optarg) + 1);
			strcpy(password, optarg);
			for (i = 0; optarg[i]; i++) {
				optarg[i] = 'x';
			}
			break;
		case 'm':
			mountpoint = optarg;
			break;
		case 'u':
			url = optarg;
			break;
		case 'U':
			user = optarg;
			break;
		case 'n':
			nazwa = optarg;
			break;
		case 'd':
			opis = optarg;
			break;
		case 'o':
			format = SHOUT_FORMAT_VORBIS;
			break;
		case '1':
			protocol = SHOUT_PROTOCOL_XAUDIOCAST;
			break;
		case 'S':
			protocol = SHOUT_PROTOCOL_ICY;
			break;
		case '?':
			if (isprint(optopt))
				fprintf(stderr, "Unknown option: -%c\n", optopt);
			else
				fprintf(stderr, "Uknown option: -<unprintable>\n");
			usage();
			return 1;
		default:
			usage();
			return 1;
		}
	}
	shout_init();
	conn = shout_new();
        if (shout_set_protocol(conn, protocol) != SHOUTERR_SUCCESS) {
                printf("Error setting protocol: %s\n", shout_get_error(conn));
                exit(1);
        }
	if (shout_set_host(conn, ip) != SHOUTERR_SUCCESS) {
		printf("Error setting hostname: %s\n", shout_get_error(conn));
		exit(1);
	}
	if (shout_set_port(conn, port) != SHOUTERR_SUCCESS) {
		printf("Error setting port: %s\n", shout_get_error(conn));
		exit(1);
	}
	if (shout_set_password(conn, password) != SHOUTERR_SUCCESS) {
		printf("Error setting password: %s\n", shout_get_error(conn));
		exit(1);
	}
	if (shout_set_mount(conn, mountpoint) != SHOUTERR_SUCCESS) {
		printf("Error setting mountpoint: %s\n", shout_get_error(conn));
		exit(1);
	}
	if (shout_set_user(conn, user) != SHOUTERR_SUCCESS) {
		printf("Error setting user: %s\n", shout_get_error(conn));
		exit(1);
	}
	if (shout_set_format(conn, format) != SHOUTERR_SUCCESS) {
		printf("Error setting format: %s\n", shout_get_error(conn));
		exit(1);
	}
	if (shout_set_name(conn, nazwa) != SHOUTERR_SUCCESS) {
		printf("Error setting name: %s\n", shout_get_error(conn));
		exit(1);
	}
	if (shout_set_url(conn, url) != SHOUTERR_SUCCESS) {
		printf("Error setting URL: %s\n", shout_get_error(conn));
		exit(1);
	}
	if (shout_set_description(conn, opis) != SHOUTERR_SUCCESS) {
		printf("Error setting description: %s\n", shout_get_error(conn));
		exit(1);
	}

	fprintf(stderr, "Server: %s@%s:%d\n", shout_get_user(conn),
		shout_get_host(conn), shout_get_port(conn));
	fprintf(stderr, "Mountpoint: %s\n", shout_get_mount(conn));

	if (shout_open(conn) != SHOUTERR_SUCCESS) {
		fprintf(stderr, "Can't connect: error %s\n", shout_get_error(conn));
		exit(1);
	}
	fprintf(stderr, "Connected\n");

	while (1) {
		read = fread(buff, 1, BUFSIZE, stdin);
		total = total + read;

		if (read > 0) {
			if (shout_send(conn, buff, read) != SHOUTERR_SUCCESS) {
				fprintf(stderr, "Send error: %s\n", shout_get_error(conn));
				break;
			}
		}
		else {
			break;
		}
		shout_sync(conn);
	}
	shout_close(conn);
	return 0;
}
