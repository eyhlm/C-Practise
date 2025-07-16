#define SAFE_FREE(ptr) do { 
    free(ptr);             
    (ptr) = NULL;          
} while(0)

//计算结果数量函数
int CalResultNumber(int numsSize)
{
    int result = 1;
    for (int i = 1; i <= numsSize; i++) {
        result *= i;
    }
    return result;
}

void BackTrackPermute(int* nums, int numsSize, int** result, int* used, int* resultPos, int curPos)
{
    int iTmp = 0;
    if (curPos == numsSize) {
        *resultPos += 1;
        memcpy(result[*resultPos], result[(*resultPos) - 1], sizeof(int) * numsSize);
        return;
    }
    for (int i = 0; i < numsSize; i++) {
        if (used[i] != 0) {
            continue;
        }
        iTmp = i;
        result[*resultPos][curPos] = nums[i];
        used[iTmp] = 1;
        BackTrackPermute(nums, numsSize, result, used, resultPos, curPos + 1);
        used[iTmp] = 0;
    }
    return;
}

int** Permute(int* nums, int numsSize, int* returnSize, int** returnColumnSizes) 
{
    int iRetSize = 0;
    int iRetPos = 0;
    int* pRetCol = NULL;
    int* used = NULL;
    int** result = NULL;

    iRetSize = CalResultNumber(numsSize);

    // 申请空间 
    result = (int**)malloc(sizeof(int*) * (iRetSize + 1));
    if (NULL == result) {
        printf("malloc result failed!\n");
        return NULL;
    }
    pRetCol = (int*)malloc(sizeof(int) * (iRetSize + 1));
    if (NULL == pRetCol) {
        printf("malloc pRetCol failed!\n");
        free(result);
        result = NULL;
        return NULL;
    }
    used = (int*)malloc(sizeof(int) * numsSize);
    if (NULL == used) {
        printf("malloc used failed!\n");
        SAFE_FREE(result);
        SAFE_FREE(pRetCol);
    }

    // 初始化
    memset(used, 0x00, sizeof(int) * numsSize);
    for (int i = 0; i <= iRetSize; i++) {
        result[i] = (int*)malloc(sizeof(int) * numsSize);
        memset(result[i], 0x00, sizeof(int) * numsSize);
        pRetCol[i] = numsSize;
    }

    // 调用回溯函数
    BackTrackPermute(nums, numsSize, result, used, &iRetPos, 0);

    // 释放空间
    free(used);

    *returnSize = iRetSize;
    *returnColumnSizes = pRetCol;
    return result;
}