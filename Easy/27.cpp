#include <vector>
#include <map>
#include <iostream>
using namespace std;

class Solution {
public:
    int removeElement(vector<int>& nums, int val) {

        int k = 0;
        int length = nums.size();

        for (auto it = nums.begin(); it != nums.end(); ++it) {
            if (*it != val) {
                nums[k] = *it;
                k++;
            }
        }

        return k;
    }
};

/*
    题目描述：
    给你一个数组 nums 和一个值 val，你需要 原地 移除所有数值等于 val 的元素。元素的顺序可能发生改变。然后返回 nums 中与 val 不同的元素的数量。

    解题思路：
    这一题我一开始的想法就有些问题，加上太久没写C++了，写起来很生疏。一开始还以为不能修改原来的数组，以及Val的值对应的数组元素的多少就是我要返回的值，总之解出来了。
*/