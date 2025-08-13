#include <iostream>
#include <vector>

using namespace std;

class Solution {
public:

    bool canJump(vector<int>& nums) {
        int jump = 0;
        int length = nums.size();
        
        for ( int i = 0; i < length; ++i) {
          if( i > jump ) return false;
          jump = max(jump, i + nums[i]);
        }
        return true;
    }
};