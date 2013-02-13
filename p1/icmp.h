#ifndef __ICMP_H
#define __ICMP_H

#define ICMP_HEADER_LEN 8

unsigned short in_cksum(const unsigned short *addr, register int len, unsigned short csum);

#endif
