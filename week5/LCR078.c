struct ListNode *MergeTowLists(struct ListNode *a, struct ListNode *b)
{
    if (NULL == a || NULL == b) {
        return a ? a : b;
    }
    struct ListNode head;
    struct ListNode *tail = &head;
    struct ListNode *p_a = a, *p_b = b;
    while (p_a && p_b) {
        if (p_a->val < p_b->val) {
            tail->next = p_a;
            p_a = p_a->next;
        } else {
            tail->next = p_b;
            p_b = p_b->next;
        }
        tail = tail->next;
    }
    tail->next = p_a ? p_a : p_b;
    return head.next;
}

struct ListNode *MergeLists(struct ListNode **lists, int left, int right)
{
    if (left == right) {
        return lists[left];
    }
    if (left > right) {
        return NULL;
    }
    int middle = (left + right) / 2;
    return MergeTowLists(MergeLists(lists, left, middle),MergeLists(lists, middle + 1, right));
}

struct ListNode *MergeKLists(struct ListNode **lists, int listsSize)
{
    return MergeLists(lists, 0, listsSize -1);
}