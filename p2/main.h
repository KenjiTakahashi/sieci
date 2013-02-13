#ifndef __MAIN_H_
#define __MAIN_H_

int is_ip(char *argv);
int first_ip_byte(char *argv);
char *get_whois_serv(int argv);
char *get_whois_info(char *serv,char *ip,int n);
void ServeClient(int fd);

#endif /* __MAIN_H_ */
