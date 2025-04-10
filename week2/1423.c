int SumFirstNPoint(int *cardPoints, int n)
{
    int sum = 0;
    for(int i = 0; i < n; i++){
        sum += cardPoints[i];
    }
    return sum;
}

int MaxScore(int *cardPoints, int cardPointsSize, int k)
{
    int leftIndex = 0;
    int rightIndex = cardPointsSize - k;
    int cardPointsSum = SumFirstNPoint(cardPoints, cardPointsSize);
    int curPointsSum = SumFirstNPoint(cardPoints, cardPointsSize - k);
    int minMiddleSum = curPointsSum;
    for(int i = 0; i < k; i++){
        curPointsSum = curPointsSum - cardPoints[leftIndex] + cardPoints[rightIndex];
        leftIndex++;
        rightIndex++;
        minMiddleSum = minMiddleSum > curPointsSum ? curPointsSum : minMiddleSum;
    }
    return cardPointsSum - minMiddleSum;
}