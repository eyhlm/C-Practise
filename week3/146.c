#include "uthash.h"

typedef struct {
    int key;
    int value;
    UT_hash_handle hh;
} singleCache;

typedef struct {
    singleCache *users;
    int capacity;
} LRUCache;

LRUCache* LRUCacheCreate(int capacity)
{
    LRUCache *obj = NULL;
    obj = (LRUCache *)malloc(sizeof(LRUCache));
    if (NULL == obj) {
        printf("malloc LRUCache failed!\n");
        return NULL;
    }
    obj->capacity = capacity;
    obj->users = NULL;
    return obj;
}

int LRUCacheGet(LRUCache *obj, int key)
{
    singleCache *cur_usr = NULL;
    HASH_FIND_INT(obj->users, &key, cur_usr);
    if (NULL != cur_usr) {
        HASH_DEL(obj->users, cur_usr);
        HASH_ADD_INT(obj->users, key, cur_usr);
        return cur_usr->value;
    }
    return -1;
}

void LRUCachePut(LRUCache *obj, int key, int value)
{
    singleCache *cur_usr = NULL, *next_usr = NULL;
    HASH_FIND_INT(obj->users, &key, cur_usr);
    if (NULL != cur_usr) {
        HASH_DEL(obj->users, cur_usr);
        cur_usr->key = key;
        cur_usr->value = value;
        HASH_ADD_INT(obj->users, key, cur_usr);
    } else {
        int cur_cap = HASH_COUNT(obj->users);
        if (cur_cap == obj->capacity) {
            HASH_ITER(hh, obj->users, cur_usr, next_usr){
                HASH_DEL(obj->users, cur_usr);
                free(cur_usr);
                cur_usr = NULL;
                break;
            }
        }
        singleCache *newCache = (singleCache *)malloc(sizeof(singleCache));
        if (NULL == newCache) {
            printf("malloc newCache failed!\n");
            return;
        }
        newCache->key = key;
        newCache->value = value;
        HASH_ADD_INT(obj->users, key, newCache);
    }
    return;
}

void LRUCacheFree(LRUCache *obj)
{
    singleCache *cur_usr = NULL, *next_usr = NULL;
    HASH_ITER(hh, obj->users, cur_usr, next_usr){
        HASH_DEL(obj->users, cur_usr);
        free(cur_usr);
        cur_usr = NULL;
    }
    free(obj);
    obj = NULL;
    return;
}