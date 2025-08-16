#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

class Solution {
public:
    int hIndex(vector<int>& citations) {
        
        int h = 0;

        std::sort(citations.begin(), citations.end());
        int length = citations.size() -1;

        for( int i = length; i >= 0; i--) {
            if (h >= citations[i] ) {
                break;
            }
            h++;
        }

        return h;
    }
};