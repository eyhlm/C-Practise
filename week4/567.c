#define FAIL_CODE -1

bool IfAllZero(int *hash_s1)
{   
    for (int i = 0; i < 26; i++) {
        if (hash_s1[i] != 0) {
            return false;
        }
    }
    return true;
}

bool CheckInclusion(char* s1, char* s2)
{
    int len_s1 = strlen(s1);
    int len_s2 = strlen(s2);
    if (len_s1 > len_s2) {
        return false;
    }
    int *hash_s1 = NULL;
    hash_s1 = (int *)malloc(26 * sizeof(int));
    memset(hash_s1, 0, 26 * sizeof(int));
    if (NULL == hash_s1) {
        printf("malloc hash_s1 failed!\n");
        return false;
    }

    for (int i = 0; i < len_s1; i++) {
        hash_s1[s1[i] - 'a']++;
        hash_s1[s2[i] - 'a']--;
    }

    if (IfAllZero(hash_s1)) {
        free(hash_s1);
        hash_s1 = NULL;
        return true;
    }

    for (int i = len_s1; i < len_s2; i++) {
        hash_s1[s2[i] - 'a']--;
        hash_s1[s2[i - len_s1] - 'a']++;
        if (IfAllZero(hash_s1)) {
            free(hash_s1);
            hash_s1 = NULL;
            return true;
        }
    }
    free(hash_s1);
    hash_s1 = NULL;
    return false;
}