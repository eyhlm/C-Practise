typedef struct {
    char **urls;
    int size;
    int capacity;
    int currIndex;
} BrowserHistory;

BrowserHistory* BrowserHistoryCreate(char *homepage)
{
    BrowserHistory* obj = (BrowserHistory*)malloc(sizeof(BrowserHistory));
    if (obj == NULL) {
        printf("malloc BrowserHistory failed\n");
        return NULL;
    }
    obj->capacity = 64;
    obj->size = 1;
    obj->currIndex = 0;
    obj->urls = (char**)malloc(obj->capacity * sizeof(char*));
    if (obj == NULL) {
        printf("malloc urls failed\n");
        free(obj);
        obj = NULL;
        return NULL;
    }
    obj->urls[0] = strdup(homepage);
    return obj;
}

void BrowserHistoryVisit(BrowserHistory *obj, char *url)
{
    while (obj->size > obj->currIndex + 1) {
        free(obj->urls[obj->size - 1]);
        obj->size--;
    }
    if (obj->size == obj->capacity) {
        obj->capacity *= 2;
        char **urlsTemp = (char**)realloc(obj->urls, obj->capacity * sizeof(char*));
        if (urlsTemp != NULL) {
            obj->urls = urlsTemp;
        } else {
            printf("realloc urls failed! This visit will not be recorded.\n");
            urlsTemp = NULL;
            return;
        }
    }
    obj->urls[obj->size] = strdup(url);
    obj->size++;
    obj->currIndex++;
    return;
}

char* BrowserHistoryBack(BrowserHistory *obj, int steps)
{
    obj->currIndex = fmax(0, obj->currIndex - steps);
    return obj->urls[obj->currIndex];
}

char* BowserHistoryForward(BrowserHistory *obj, int steps)
{
    obj->currIndex = fmin(obj->size - 1, obj->currIndex + steps);
    return obj->urls[obj->currIndex];
}

void BrowserHistoryFree(BrowserHistory *obj)
{
    for (int i = 0; i < obj->size; i++) {
        free(obj->urls[i]);
        obj->urls[i] = NULL;
    }
    free(obj->urls);
    obj->urls = NULL;
    free(obj);
    obj = NULL;
    return;
}