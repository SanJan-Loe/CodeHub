#include<iostream>
#include <vector>
#include <stack>

using namespace std;

class Solution {
public:
    int trap(vector<int>& height) {
        if (height.size() < 2) {
            return 0;
        }

        int rainCount = 0,stackCount = 0;
        int leftMax = height[0];
        int len = height.size();

        for(int i = 1; i < len; ++i) {
            if(height[i] < leftMax) {
                stackCount += (leftMax - height[i]);
            } else {
                leftMax = height[i];
                rainCount += stackCount;
                stackCount = 0;
            }
        }

        return rainCount;
    }

    int trapB(vector<int>& height)
    {
        int ans = 0;
        stack<int> st;
        for (int i = 0; i < height.size(); i++)
        {
            while (!st.empty() && height[st.top()] < height[i])
            {
                int cur = st.top();
                st.pop();
                if (st.empty()) break;
                int l = st.top();
                int r = i;
                int h = min(height[r], height[l]) - height[cur];
                ans += (r - l - 1) * h;
            }
            st.push(i);
        }
        return ans;
    }

};