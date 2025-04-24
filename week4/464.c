#define VISITED_AND_TRUE 1
#define VISITED_AND_FALSE 2

bool Dfs(int *visited, int state, int sum, int maxChoosableInteger, int desiredTotal)
{
    if (visited[state] == VISITED_AND_TRUE) {
        return true;
    }
    if (visited[state] == VISITED_AND_FALSE) {
        return false;
    }
    for (int i = 1; i <= maxChoosableInteger; i++) {
        if ((1 << i - 1) & state == 1) {
            continue;
        }
        if (sum + i >= desiredTotal) {
            visited[state] = VISITED_AND_TRUE;
            return true;
        }

        if (!Dfs(visited, ((1 << i - 1) | state), sum + i, maxChoosableInteger, desiredTotal)) {
            visited[state] = VISITED_AND_TRUE;
            return true;
        }
    }
    visited[state] = VISITED_AND_FALSE;
    return false;
}

bool CanIWin(int maxChoosableInteger, int desiredTotal)
{
    if (maxChoosableInteger >= desiredTotal) {
        return true;
    }
    if (maxChoosableInteger * (maxChoosableInteger + 1) / 2 < desiredTotal) { // 对等差数列求和
        return false;
    }

    int *visited = NULL;
    visited = (int *)malloc((1 << maxChoosableInteger) * sizeof(int));
    memset(visited, 0, (1 << maxChoosableInteger) * sizeof(int));
    if (NULL == visited) {
        printf("malloc visited failed!\n");
        return faldse;
    }
    return Dfs(visited, 0, 0, maxChoosableInteger, desiredTotal);
}
