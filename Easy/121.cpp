#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:
    int maxProfit(vector<int>& prices) {
        int profit = 0;
        int lowest = prices[0];
        int length = prices.size();

        for (int i = 0; i < length; i++) {
            lowest = min(lowest, prices[i]);
            profit = max(profit, (prices[i] - lowest));
        }

        return profit;
    }
};

int main() {


    return 0;
}