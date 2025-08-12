#include <iostream>
#include <vector>
#include <map>

using namespace std;

class Solution {
public:
    int majorityElement(vector<int>& nums) {
        map<int, int> counts; // key: number, value:

        for (auto num : nums ) {
            counts[num]++;
        }

        for ( auto numCount : counts) {
            if (numCount.second > (nums.size() /2)){
                return numCount.first;
            }
        }

        return 0;
    }
};