bool IsCompleteTree(struct TreeNode* root)
{
    struct TreeNode **treeNodeQuene = NULL;
    struct TreeNode *tmp = NULL;
    // 树中节点数目在范围 [1, 100] 内，最后一层最多37个节点，层序遍历需要37 * 2冗余
    treeNodeQuene = (struct TreeNode **)malloc(201 * sizeof(struct TreeNode *)); 
    if (NULL == treeNodeQuene) {
        printf("malloc treeNodeQuene failde!\n");
        return false;
    }
    int front = 0, cur = 0;
    treeNodeQuene[cur++] = root;
    while (front < cur) {
        tmp = treeNodeQuene[front++];
        if (tmp) {
            treeNodeQuene[cur++] = tmp->left;
            treeNodeQuene[cur++] = tmp->right;
        } else {
            while (front < cur) {
                tmp = treeNodeQuene[front++];
                if (tmp) {
                    return false;
                }
            }
        }
    }
    return true;
}