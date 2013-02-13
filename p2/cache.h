#ifndef __CACHE_H_
#define __CACHE_H_

typedef struct CACHE
{
    int size;
    char *key;
    char *value;
} cache;

void cache_init();
void cache_add(char *key,char *value);
char *cache_get(char *key);
void cache_free();

#endif /* __CACHE_H_ */
