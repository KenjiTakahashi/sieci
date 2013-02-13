/* Karol Woźniak, 233234 */
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "icmp.h"

int main(int argc,char *argv[])
{
	// Sprawdzamy, czy wprowadzono argument.
	if(argc!=2)
	{
		printf("Usage: ./traceroute <ip>\n");
		return 1;
	}
	// Sprawdzamy, czy podano adres IP.
	char tmp[3]={0,0,0};
	int j=0;
	int k=0;
	for(int i=0;i<strlen(argv[1]);++i)
	{
		if(argv[1][i]>='0'&&argv[1][i]<='9')
		{
			tmp[j++]=argv[1][i];
			tmp[j]=0;
		}
		else if(argv[1][i]=='.')
		{
			if(atoi(tmp)<=255)
			{
				k++;
				j=0;
				if(i+1==strlen(argv[1]))
					k++;
			}
			else
			{
				printf("It doesn't look like an IP address.\n");
				return 1;
			}
		}
		else
		{
			printf("It doesn't look like an IP address.\n");
			return 1;
		}
	}
	if(atoi(tmp)>255||k!=3)
	{
		printf("It doesn't look like an IP address.\n");
		return 1;
	}
	// Tworzymy socket raw.
	int sockfd=socket(AF_INET,SOCK_RAW,IPPROTO_ICMP);
	if(sockfd<0)
	{
		printf("Cannot properly set socket.\nRemember: You have to be root to do this.\n");
		return 1;
	}
	int ttl=0;
	int retry;
	unsigned char buffer[IP_MAXPACKET+1];
	unsigned char* ptr;
	struct icmphdr packet;
	struct sockaddr_in sender;
	char str[20];
	struct iphdr *ipp;
	struct icmphdr *icmph;
	struct timeval tv;
	fd_set readfd;
	socklen_t length=sizeof(sender);
	// Tworzymy strukture adresu docelowego.
	struct sockaddr_in addr;
	memset(&addr,0,sizeof(addr));
	addr.sin_family=AF_INET;
	inet_pton(AF_INET,argv[1],&addr.sin_addr);
	// Tworzymy strukture pakietu ICMP.
	packet.type=ICMP_ECHO;
	packet.code=0;
	packet.un.echo.id=666;
	packet.un.echo.sequence=1;
	packet.checksum=0;
	packet.checksum=in_cksum((unsigned short*)&packet,8,0);
	// Wysylamy zapytania do kolejnych adresow, tak dlugo, jak dlugo otrzymujemy pakiety typu ICMP_TIME_EXCEEDED (max. 50 przeskokow).
	do
	{
		// Resetujemy ustawienia dla kolejnego pakietu.
		retry=0;
		FD_ZERO(&readfd);
		FD_SET(sockfd,&readfd);
		// Ustawiamy TTL.
		if(setsockopt(sockfd,IPPROTO_IP,IP_TTL,&ttl,sizeof(int))<0)
		{
			printf("Cannot set TTL for socket.\n");
			return 1;
		}
		ptr=buffer;
		// Probujemy wyslac pakiet, w przypadku niepowodzenia powtarzamy operacje trzykrotnie.
        do
        {
			tv.tv_sec=2;
			if(sendto(sockfd,&packet,ICMP_HEADER_LEN,0,(struct sockaddr*)&addr,sizeof(addr))!=ICMP_HEADER_LEN)
			{
				printf("Cannot send packet.\nCheck your internet connection.\n");
				return 1;
			}
			select(sockfd+1,&readfd,NULL,NULL,&tv);
		}
		while(++retry<3&&!FD_ISSET(sockfd,&readfd));
		// Jesli dostalismy odpowiedz.
		if(FD_ISSET(sockfd,&readfd))
		{
			// To wyciagamy z niej adres ip komputera, z ktorego zostala wyslana.
			if(recvfrom(sockfd,ptr,IP_MAXPACKET,0,(struct sockaddr*)&sender,&length)<0)
			{
				printf("Cannot receive packet.\nCheck your internet connection.");
				return 1;
			}
			inet_ntop(AF_INET,&(sender.sin_addr),str,sizeof(str));
			printf("%i. %s\n",++ttl,str);
			ipp=(struct iphdr*)ptr;
			ptr+=ipp->ihl*4; // Pomijamy naglowek IP.
			icmph=(struct icmphdr*)ptr; // Odbieramy ICMP, aby sprawdzic, czy dostalimy odp. od routera czy komputera docelowego.
		}
		else
		{
			printf("%i. *\n",++ttl); // Jesli nie dostalismy odpowiedzi, wyswietlamy '*' i idziemy dalej.
			icmph=NULL; // Dla spokojnosci :)
		}
	}
	while(ttl<50&&(icmph==NULL||(icmph->type==ICMP_TIME_EXCEEDED&&icmph->code==ICMP_EXC_TTL)));
	// Sprawdzamy, czy pakiet faktycznie dotarl do celu.
	if(icmph->type!=ICMP_ECHOREPLY)
		printf("Wrong ICMP message received.\n");
    return 0;
}
