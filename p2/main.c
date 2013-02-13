/* Karol Wozniak, 233234 */

#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include "sockwrap.h"
#include "tcpconn.h"
#include "readchunk.h"
#include "cache.h"
#include "main.h"

#define MAXLINE 100
#define BUFFSIZE 10000000

// Sprawdzamy, czy podano adres IP.
int is_ip(char *argv)
{
    char tmp[3]={0,0,0};
	int j=0;
	int k=0;
	for(int i=0;i<strlen(argv)-2;++i)
	{
		if(argv[i]>='0'&&argv[i]<='9')
		{
			tmp[j++]=argv[i];
			tmp[j]=0;
		}
		else if(argv[i]=='.')
		{
			if(atoi(tmp)<=255)
			{
				k++;
				j=0;
				if(i+1==strlen(argv))
					k++;
			}
			else
				return 1;
		}
		else
			return 1;
	}
	if(atoi(tmp)>255||k!=3)
		return 1;
    return 0;
}

// Pobieramy pierwszy bajt adresu (zeby wyznaczyc serwer whois).
int first_ip_byte(char *argv)
{
    char tmp[3]={0,0,0};
    int j=0;
    for(int i=0;i<3;++i)
    {
        if(argv[i]!='.')
            tmp[j++]=argv[i];
    }
    return atoi(tmp);
}

// Wyznaczamy serwer whois.
char *get_whois_serv(int argv)
{
    char *serv;
    if(argv==1||argv==14||argv==27||(argv>=58&&argv<=61)||(argv>=110&&argv<=126)||argv==133||(argv>=150&&argv<=153) \
    ||argv==163||argv==173||argv==175||argv==180||argv==182||argv==183||argv==202||argv==210||argv==211||(argv>=218&&argv<=223))
        serv="202.12.29.220";
    else if(argv==2||argv==25||argv==31||argv==46||argv==51||argv==62||(argv==77&&argv==95)||argv==109||argv==141||argv==145||argv==151 \
    ||argv==176||argv==178||argv==188||(argv>=193&&argv<=195)||argv==212||argv==213||argv==217)
        serv="193.0.6.135";
    else if(argv==41||argv==154||argv==196||argv==197)
        serv="196.216.2.1";
    else if(argv==186||argv==187||(argv>=189&&argv<=191)||argv==200||argv==201)
        serv="200.3.14.10";
    else
        serv="199.71.0.43";
    return serv;
}

char *get_whois_info(char *serv,char *ip,int n)
{
    char *buffer=calloc(BUFFSIZE+1,1);
    int sockfd=TCPConnect(serv,43);
    if(sockfd<0)
    {
        strcpy(buffer,"Could not connect to server.\nCheck your internet connection.\n");
        return buffer;
    }
    WriteAll(sockfd,ip,n);
    shutdown(sockfd,SHUT_WR);
    if(ReadChunk2(sockfd,buffer,BUFFSIZE,EOF,30)<=0)
        strcpy(buffer,"Could not retrieve information from server.\nCheck back later.\n");
    return buffer;
}

// Funkcja obslugujaca klienta
void ServeClient(int fd)
{
	InitBuffer();
	int n;
    char buffer[MAXLINE+1];
    // wczytujemy jeden wiersz 
	// Czekamy na dane wejsciowe do 10 sekund.
	while((n=ReadChunk2(fd,buffer,MAXLINE,'\n',10))>0)
    {
        if(is_ip(buffer)!=0)
            WriteAll(fd,"It's not an ip address!\n",24);
        else
        {
            char *response;
            if((response=cache_get(buffer))==NULL)
            {
                response=get_whois_info(get_whois_serv(first_ip_byte(buffer)),buffer,n);
                cache_add(buffer,response);
            }
            WriteAll(fd,response,strlen(response));
            free(response);
        }
	}
	if(n==-1)
        printf("Timeout!\n");
	Close(fd);
	printf("Client disconnected\n");
}

int main()
{
	int sockfd=StartTCPServer(34343);
    cache_init();
    printf("Server started.\nType Ctrl-D to terminate.\n");
	while(1)
    {
		int conn_sockfd=MyAccept(sockfd);
        if(conn_sockfd==-2)
            break;
		ServeClient(conn_sockfd);
	}
    cache_free();
    return 0;
}
