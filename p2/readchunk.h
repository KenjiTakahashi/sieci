#ifndef __READCHUNK_H_
#define __READCHUNK_H_

void InitBuffer ();
int ReadChunk (int sockfd, char* str, int maxlen, char delimiter);
int ReadChunk2 (int sockfd, char* str, int maxlen, char delimiter, int seconds);

#endif
