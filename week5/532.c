#include "uthash.h"

typedef struct {
    int key;
    UT_hash_handle hh;
} HashItem;

int ProcessHash(int target,int curVal, HashItem **res, HashItem **visited)
{
    HashItem *cur_usr = NULL;
    int pairMin = fmin(target, curVal);
    HASH_FIND_INT(*visited, &target, cur_usr);
    if (cur_usr) {
        cur_usr = NULL;
        HASH_FIND_INT(*res, &pairMin, cur_usr);
        if (NULL == cur_usr) {
            printf("1\n");
            HashItem *temp = (HashItem *)malloc(sizeof(HashItem));
            if (NULL == temp) {
                printf("malloc HashItem failed!\n");
                return -1;
            }
            temp->key = pairMin;
            HASH_ADD_INT(*res, key, temp);
        }
    }
    return 0;
}

int FindPairs(int *nums, int numsSize, int k)
{
    HashItem *res = NULL, *visited = NULL;
    for (int i = 0; i < numsSize; i++) {
        HashItem *cur_usr = NULL;
        ProcessHash(nums[i] - k, nums[i], &res, &visited);
        ProcessHash(nums[i] + k, nums[i], &res, &visited);
        
        cur_usr = NULL;
        HASH_FIND_INT(visited, &nums[i], cur_usr);
        if (NULL == cur_usr) {
            HashItem *temp = (HashItem *)malloc(sizeof(HashItem));
            if (NULL == temp) {
                printf("malloc HashItem failed!\n");
                return -1;
            }
            temp->key = nums[i];
            HASH_ADD_INT(visited, key, temp);
        }
    }
    return HASH_COUNT(res);
}