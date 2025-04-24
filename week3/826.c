#define FAIL_CODE -1

int GetMaxDifficulty(int *difficulty, int difficultySize)
{
    int max = 0;
    for (int i = 0; i < difficultySize; i++) {
        if (difficulty[i] > max) {
            max = difficulty[i];
        }
    }
    return max;
}

int MaxProfitAssignment(int *difficulty, int difficultySize, int *profit, int profitSize, int *worker, int workerSize)
{
    int result = 0;
    int maxDifficulty = GetMaxDifficulty(difficulty, difficultySize);
    int *dp = (int*)malloc((maxDifficulty + 1) * sizeof(int));
    if (NULL == dp) {
        printf("malloc dp failed!\n");
        dp = NULL;
        return FAIL_CODE;
    }
    memset(dp, 0, (maxDifficulty + 1) * sizeof(int));
    
    for (int i = 0; i < difficultySize; i++) {
        dp[difficulty[i]] = profit[i] > dp[difficulty[i]] ? profit[i] : dp[difficulty[i]]; // 相同的difficulty保留收益最高的结果
    }
    int curMax = 0;
    for (int i = 0; i < maxDifficulty + 1; i++) { // dp数组下标与difficulty对齐
        if (curMax < dp[i]) {
            curMax = dp[i];
        }
        dp[i] = curMax;
    }
    for (int i = 0; i < workerSize; i++) {
        if (worker[i] > maxDifficulty) {
            result += dp[maxDifficulty];
        } else {
            result += dp[worker[i]];
        }
    }

    free(dp);
    dp = NULL;
    return result;
}