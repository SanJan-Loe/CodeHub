#include <cstdlib>
#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;


class RandomizedSet {
private:
    vector<int> arr;
    unordered_map<int, int> hash;

public:
    RandomizedSet() { }
    
    bool insert(int val) {
        if ( hash.find(val) != hash.end() ) return false;
        hash[val] = arr.size();
        arr.push_back(val);
        return true;
    }
    
    bool remove(int val) {
        if (hash.find(val) == hash.end() ) return false;
        int index = hash[val];
        hash[arr.back()] = index;
        arr[index] = arr.back();
        arr.pop_back();
        hash.erase(val);
        return true;
    }
    
    int getRandom() {
        return arr[rand() % arr.size()];
    }
};

/**
 * Your RandomizedSet object will be instantiated and called as such:
 * RandomizedSet* obj = new RandomizedSet();
 * bool param_1 = obj->insert(val);
 * bool param_2 = obj->remove(val);
 * int param_3 = obj->getRandom();
 */