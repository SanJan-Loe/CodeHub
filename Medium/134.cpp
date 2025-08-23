#include <vector>
using namespace std;

class Solution {
public:
    int canCompleteCircuit(vector<int>& gas, vector<int>& cost) {
        // 总油量和总消耗，用于判断是否存在解
        int totalGas = 0;
        int totalCost = 0;
        
        // 当前油量，用于跟踪行驶过程中的油量变化
        int currentGas = 0;
        
        // 起始加油站索引
        int start = 0;
        
        // 遍历所有加油站
        for (int i = 0; i < gas.size(); i++) {
            // 累计总油量和总消耗
            totalGas += gas[i];
            totalCost += cost[i];
            
            // 计算在当前加油站加油后，前往下一加油站剩余的油量
            currentGas += gas[i] - cost[i];
            
            // 如果当前油量不足，说明从start到i的任意点都无法作为起点
            // 因为从start开始，经过所有中间加油站后油量都已不足
            // 所以起始点必须在i+1之后
            if (currentGas < 0) {
                // 更新起始点为下一个加油站
                start = i + 1;
                // 重置当前油量，从新的起始点重新计算
                currentGas = 0;
            }
        }
        
        // 如果总油量小于总消耗，则无解，返回-1
        // 否则，start就是唯一的起始点
        return totalGas >= totalCost ? start : -1;
    }
};