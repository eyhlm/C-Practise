
//计算结果数量函数
int CalResultNumber(int numsSize)
{
    int result = 1;
    for(int i = 1; i <= numsSize; i++){
        result *= i;
    }
    return result;
}

void BackTrackPermute(int* nums, int numsSize, int** pRet, int* pUsed, int* pRetPos, int iCurPos)
{
    int iTmp = 0;
    if(iCurPos == numsSize){
        *pRetPos += 1;
        memcpy(pRet[*pRetPos], pRet[(*pRetPos) - 1], sizeof(int) * numsSize);
        return;
    }
    for (int i = 0; i < numsSize; i++){
        if (pUsed[i] == 0)
        {
            iTmp = i;
            pRet[*pRetPos][iCurPos] = nums[i];
            pUsed[iTmp] = 1;
            BackTrackPermute(nums, numsSize, pRet, pUsed, pRetPos, iCurPos + 1);
            pUsed[iTmp] = 0;
        }
    }
    return;
}

int** Permute(int* nums, int numsSize, int* returnSize, int** returnColumnSizes) 
{
    int iRetSize = 0;
    int iRetPos = 0;
    int* pRetCol = NULL;
    int* pUsed = NULL;
    int** pRet = NULL;

    iRetSize = CalResultNumber(numsSize);

    //申请空间 
    pRet = (int**)malloc(sizeof(int*) * (iRetSize + 1));
    pRetCol = (int*)malloc(sizeof(int) * (iRetSize + 1));
    pUsed = (int*)malloc(sizeof(int) * numsSize);
    if(pRet == NULL || pRetCol == NULL || pUsed == NULL){
        printf("malloc failed!\n");
        return NULL;
    }

    //初始化
    memset(pUsed, 0x00, sizeof(int) * numsSize);
    for(int i = 0; i <= iRetSize; i++){
        pRet[i] = (int*)malloc(sizeof(int) * numsSize);
        memset(pRet[i], 0x00, sizeof(int) * numsSize);
        pRetCol[i] = numsSize;
    }

    //调用回溯函数
    BackTrackPermute(nums, numsSize, pRet, pUsed, &iRetPos, 0);

    //释放空间
    free(pUsed);

    *returnSize = iRetSize;
    *returnColumnSizes = pRetCol;
    return pRet;
}