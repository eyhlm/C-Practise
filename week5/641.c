typedef struct Node {
    int key;
    struct Node *next;
    struct Node *front;
} Node;


typedef struct {
    int max_capacity;
    int cur_capacity;
    Node *head;
    Node *tail;
} MyCircularDeque;


MyCircularDeque *myCircularDequeCreate(int k)
{
    MyCircularDeque *p = NULL;
    p = (MyCircularDeque *)malloc(sizeof(MyCircularDeque));
    if (NULL == p) {
        printf("malloc MyCircularDeque failed!\n");
        return NULL;
    }
    p->max_capacity = k;
    p->cur_capacity = 0;
    p->head = NULL;
    p->tail = NULL;
    return p;
}

bool myCircularDequeInsertFront(MyCircularDeque* obj, int value)
{
    if (obj->cur_capacity >= obj->max_capacity) {
        return false;
    }
    Node *temp = NULL;
    temp = (Node *)malloc(sizeof(Node));
    if (NULL == temp) {
        printf("malloc Node failed!\n");
        return false;
    }
    temp->key = value;
    temp->front = NULL;
    temp->next = NULL;
    if (0 == obj->cur_capacity) {
        obj->tail = obj->head = temp;
    } else {
        temp->next = obj->head;
        obj->head->front = temp;
        obj->head = temp;
    }
    obj->cur_capacity++;
    return true;
}

bool myCircularDequeInsertLast(MyCircularDeque *obj, int value)
{
    if (obj->cur_capacity >= obj->max_capacity) {
        return false;
    }
    Node *temp = NULL;
    temp = (Node *)malloc(sizeof(Node));
    if (NULL == temp) {
        printf("malloc Node failed!\n");
        return false;
    }
    temp->key = value;
    temp->front = NULL;
    temp->next = NULL;
    if (0 == obj->cur_capacity) {
        obj->head = obj->tail = temp;
    } else {
        temp->front = obj->tail;
        obj->tail->next = temp;
        obj->tail = temp;
    }
    obj->cur_capacity++;
    return true;
}

bool myCircularDequeDeleteFront(MyCircularDeque *obj)
{
    if (0 == obj->cur_capacity) {
        return false;
    }
    Node *temp = obj->head;
    obj->head = obj->head->next;
    if (obj->head) {
        obj->head->front = NULL;
    }
    free(temp);
    temp = NULL;
    obj->cur_capacity--;
    return true;
}

bool myCircularDequeDeleteLast(MyCircularDeque *obj)
{
    if (0 == obj->cur_capacity) {
        return false;
    }
    Node *temp = obj->tail;
    obj->tail = obj->tail->front;
    if (obj->tail) {
        obj->tail->next = NULL;
    }
    free(temp);
    temp = NULL;
    obj->cur_capacity--;
    return true;
}

int myCircularDequeGetFront(MyCircularDeque *obj)
{
    if (0 == obj->cur_capacity) {
        return -1;
    }
    return obj->head->key;
}

int myCircularDequeGetRear(MyCircularDeque *obj)
{
    if (0 == obj->cur_capacity) {
        return -1;
    }
    return obj->tail->key;
}

bool myCircularDequeIsEmpty(MyCircularDeque *obj)
{
    if (0 == obj->cur_capacity) {
        return true;
    } else {
        return false;
    }
}

bool myCircularDequeIsFull(MyCircularDeque *obj)
{
    if (obj->cur_capacity == obj->max_capacity) {
        return true;
    } else {
        return false;
    }
}

void myCircularDequeFree(MyCircularDeque *obj)
{
    Node *cur = obj->head;
    while (cur) {
        Node *temp = cur->next;
        free(cur);
        cur = temp;
    }
    free(obj);
}

/**
 * Your MyCircularDeque struct will be instantiated and called as such:
 * MyCircularDeque* obj = myCircularDequeCreate(k);
 * bool param_1 = myCircularDequeInsertFront(obj, value);
 
 * bool param_2 = myCircularDequeInsertLast(obj, value);
 
 * bool param_3 = myCircularDequeDeleteFront(obj);
 
 * bool param_4 = myCircularDequeDeleteLast(obj);
 
 * int param_5 = myCircularDequeGetFront(obj);
 
 * int param_6 = myCircularDequeGetRear(obj);
 
 * bool param_7 = myCircularDequeIsEmpty(obj);
 
 * bool param_8 = myCircularDequeIsFull(obj);
 
 * myCircularDequeFree(obj);
*/