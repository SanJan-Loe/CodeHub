#include <vector>

using namespace std;

class Solution {
public:
    int jump(vector<int>& nums) {
        
        int length = nums.size() - 1;
        int maxJump = 0;
        int end = 0;
        int steps = 0;

        for( int i = 0; i < length; ++i) {
          
            maxJump = max(maxJump, i + nums[i]);
            if( i == end ) {
                end = maxJump;
                steps++;
            }
            if( end >= length ) break;
        }

        return steps;
    }
};