int Cmp(const void *a, const void *b) 
{
    int num1 = *(const int *)a;
    int num2 = *(const int *)b;
    return num2 - num1; // 降序排序
}

int HIndex(int *citations, int citationsSize) 
{
    qsort(citations, citationsSize, sizeof(int), Cmp);
    int h = 0;
    for (int i = 0; i < citationsSize && citations[i] > h; i++) {
        h++;
    }
    return h;
}