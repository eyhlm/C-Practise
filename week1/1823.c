//大驼峰、函数括号换行、缩进用空格
int FindTheWinner(int numMember, int count) 
{
    int winner = 1;
    for (int i = 2; i <= numMember; i++) {
        winner = (count + winner - 1) % i + 1;
    }
    return winner;
}