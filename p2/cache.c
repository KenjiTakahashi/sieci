#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cache.h"

cache *c;

void cache_init()
{
    c=malloc(sizeof(struct CACHE));
    c->size=0;
}

void cache_add(char *key,char *value)
{
    printf("DEBUG (Cache): Added info to local cache.\n");
    c=realloc(c,(c->size+1)*sizeof(struct CACHE));
    c[c->size].key=calloc(strlen(key)+1,1);
    strcpy(c[c->size].key,key);
    c[c->size].value=calloc(strlen(value)+1,1);
    strcpy(c[c->size++].value,value);
}

char *cache_get(char *key)
{
    for(int i=0;i<c->size;++i)
    {
        if(strcmp(key,c[i].key)==0)
        {
            printf("DEBUG (Cache): Got info from local cache.\n");
            char *value=calloc(strlen(c[i].value)+1,1);
            strcpy(value,c[i].value);
            return value;
        }
    }
    return NULL;
}

void cache_free()
{
    for(int i=0;i<c->size;++i)
    {
        free(c[i].key);
        free(c[i].value);
    }
    free(c);
}
