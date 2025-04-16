#define ERROR_CODE -1

int MinimumTotal(int** triangle, int triangleSize, int* triangleColSize) 
{
    if (triangleSize == 1) {
        return triangle[0][0];
    }

    int* resArray = NULL;
    
    // 分配内存
    resArray = (int*)malloc(sizeof(int) * triangleColSize[triangleSize - 1]);
    if (NULL == resArray) {
        printf("malloc resArray error!\n");
        return ERROR_CODE;
    }

    // 初始化
    memcpy(resArray,triangle[triangleSize-1],
        sizeof(int) * triangleColSize[triangleSize - 1]);

    for (int i = triangleSize-2; i >= 0; i--) { // 从每行倒数第二个数字开始计算
        for (int j = 0; j < triangleColSize[i]; j++) {
            int min = resArray[j] < resArray[j+1] ? resArray[j] : resArray[j+1];
            resArray[j] = triangle[i][j] + min;
        }
    }
    int res = resArray[0];    
    // 释放内存
    free(resArray);
    return res;
}