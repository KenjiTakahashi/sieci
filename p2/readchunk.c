#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include <assert.h>
#include "sockwrap.h"
#include "tcpconn.h"

#define ERROR(str) { fprintf(stderr, "%s: %s\n", str, strerror(errno)); exit(1); }

/*
 * Bufor zwiazany z pojedynczym gniazdem
 */
static char buffer[4096];
static int buffer_len;		// ile jest wszystkich danych w buforze
static int buffer_cnt;		// ile jest przeczytanych danych w buforze

void InitBuffer()
{
	buffer_len = buffer_cnt = 0;
}

/* 
 * Czyta jeden bajt z gniazda
 * 
 * Z gniazda pobierana jest cala porcja danych i pozniej 
 * program bierze z niej po bajcie
 */
static int ReadBufferedByte (int sockfd, char* c)
{
	if (buffer_cnt == buffer_len) {
		buffer_cnt = 0;
		buffer_len = read(sockfd, buffer, 4096);
		if (buffer_len <= 0) return buffer_len;	// -1 = blad, 0 = gniazdo zamkniete
	}
	*c = buffer[buffer_cnt++]; 
	return 1;
}

/*
 * Wczytuje z gniazda kawalek danych zakonczony okreslonym znakiem.
 * Przykladowo ustawienie wartosci delimiter na '\n' wczytuje caly wiersz.
 * 
 * W przekazywanym buforze powinno byc co najmniej maxlen+1 wolnych bajtow
 * Blokuje do momentu wczytania znaku delimiter albo osiagniecia maxlen bajtow
 * Zwraca liczbę faktycznie odczytanych bajtów, 0 oznacza koniec pliku.
 * Po danych zapisywany jest znak 0. 
 */
int ReadChunk (int sockfd, char* str, int maxlen, char delimiter)
{
	int cnt = 0;	// liczba przeczytanych znakow
	while (cnt < maxlen) {
		// wczytuje jeden bajt z gniazda, blokuje jesli nie ma nic do odczytu
		int bytes_read = ReadBufferedByte (sockfd, &str[cnt]);
		if (bytes_read < 0) ERROR("readchunk error");
		if (bytes_read == 0) break;
		assert(bytes_read == 1);
		cnt++;
		if (str[cnt-1] == delimiter) break;
	}
	str[cnt] = 0;
	return cnt;
}

// Pusta procedura obslugi przerwania ALRM
static void sig_alarm( __attribute__((unused)) int sig_no) {}

int ReadChunk2 (int sockfd, char* str, int maxlen, char delimiter, int seconds)
{
	// Wlaczamy alarm za seconds sekund i ustawiamy, ze powinna sie wtedy wywolac
	// procedura sig_alarm. Procedura jest pusta, ale samo wywolanie sygnalu 
	// powoduje przerwanie funkcji read z kodem bledu EINTR.
	struct sigaction act, oldact;
	act.sa_handler = sig_alarm;
	sigemptyset (&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGALRM, &act, &oldact);
	alarm(seconds);

	int cnt = 0;	// liczba przeczytanych znakow
	while (cnt < maxlen) {
		// wczytuje jeden bajt z gniazda, blokuje jesli nie ma nic do odczytu
		int bytes_read = ReadBufferedByte (sockfd, &str[cnt]);
		if (bytes_read < 0) {
			if (errno == EINTR) {
				return -1;	// timeout
			} else {
				ERROR("readchunk error");
			}
		}
		if (bytes_read == 0) break;
		assert(bytes_read == 1);
		cnt++;
		if (str[cnt-1] == delimiter) break;
	}

	// Wylaczamy budzik i ustawiamy stara procedure obslugi sygnalu
	sigaction(SIGALRM, &oldact, NULL);
	alarm(0);

	str[cnt] = 0;
	return cnt;
}
