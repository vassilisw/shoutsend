#include <shout/shout.h>
#include <stdio.h>
#include <unistd.h>

#define BUFSIZE 4096

void usage() {
	fprintf(stderr, "Uzycie: mp3send [-s adres] [-p port] [-P haslo] [-m punkt_montowania] [-h]\n");
	fprintf(stderr, "\t[-n nazwa] [-d opis] [-u url]\n");
}

int main (int argc, char **argv) {
	char *ip = "127.0.0.1";
	int port = 8000;
	char *password = "letmein";
	char *mountpoint = "/live";
	char *nazwa = "";
	char *opis = "";
	char *url = "";
	int total, read, ret, c;
	char buff[BUFSIZE+2];
	shout_conn_t conn;

	while ((c = getopt(argc, argv, "hs:p:P:m:n:d:u:")) != -1) {
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
		case 'n':
			nazwa = optarg;
			break;
		case 'd':
			opis = optarg;
			break;
		case '?':
			if (isprint(optopt))
				fprintf(stderr, "Nieznana opcja: -%c\n", optopt);
			else
				fprintf(stderr, "Niedozwolony znak jako opcja\n");
			usage();
			return 1;
		default:
			usage();
			return 1;
		}
	}
	fprintf(stderr, "Serwer: %s:%d\n", ip, port);
	fprintf(stderr, "Mountpoint: %s\n", mountpoint);
	shout_init_connection(&conn);
	conn.ip = ip;
	conn.port = port;
	conn.password = password;
	conn.mount = mountpoint;
	conn.name = nazwa;
	conn.description = opis;
	conn.url = url;
	if (!shout_connect(&conn)) {
		fprintf(stderr, "Can't connect: error %d\n", conn.error);
		exit(1);
	}
	fprintf(stderr, "Connected\n");

	while (1) {
		read = fread(buff, 1, BUFSIZE, stdin);
		total = total + read;

		if (read > 0) {
			if (!shout_send_data(&conn, buff, read)) {
				fprintf(stderr, "Send error: %d\n", conn.error);
				break;
			}
		} else {
			break;
		}
		shout_sleep(&conn);
	}
	shout_disconnect(&conn);
	return 0;
}
