#include <vector>
#include <cstdlib>

using namespace std;

class Solution {
public:
    int removeDuplicates(vector<int>& nums) {
        int pre = 0;
        int length = nums.size();
        int max = __INT_MAX__;


        for (int i = 2; i < length; ++i) {
            if (nums[pre] != nums[i] ) {
                pre = i;
            }

            if ((i - pre) > 1) {                
                nums[pre] = max;
                pre++;
            }
        }
        
        for (auto it = nums.begin(); it != nums.end(); ) {
            if (*it == max) {
               it =  nums.erase(it);
            } else {
                ++it;
            }
        }


        return nums.size();
    }
};