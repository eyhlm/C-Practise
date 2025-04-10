// 找寻中间节点
struct ListNode* FindMiddleNode(struct ListNode *head)
{
    struct ListNode *p_slow = head;
    struct ListNode *p_fast = head;
    while(p_fast->next != NULL && p_fast->next->next != NULL){
        p_slow = p_slow->next;
        p_fast = p_fast->next->next;
    }
    return p_slow;
}
// 反转链表
struct ListNode* ReverseList(struct ListNode *head)
{
    struct ListNode *p_pre = NULL;
    struct ListNode *p_cur = head;
    while(p_cur != NULL){
        struct ListNode *p_temp = p_cur->next;
        p_cur->next = p_pre;
        p_pre =  p_cur;
        p_cur = p_temp;
    }
    return p_pre;
}

void ReorderList(struct ListNode *head) 
{
    struct ListNode *p_middle = FindMiddleNode(head);
    struct ListNode *list_1 = head;
    struct ListNode *list_2 = p_middle->next;
    p_middle->next = NULL;
    list_2 = ReverseList(list_2);
    struct ListNode *list_1_cur = list_1;
    struct ListNode *list_2_cur = list_2;
    while(list_2_cur != NULL){
        struct ListNode *l_1_temp = list_1_cur->next;
        struct ListNode *l_2_temp = list_2_cur->next;
        list_1_cur->next = list_2_cur;
        list_2_cur->next = l_1_temp;
        list_1_cur = l_1_temp;
        list_2_cur = l_2_temp;
    }
    return;
}