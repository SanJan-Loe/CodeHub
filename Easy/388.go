package easy

import "C"
import "math/rand"

type RandomizedSet struct {
	nums    []int
	indices map[int]int
}

func Constructor() RandomizedSet {
	return RandomizedSet{
		nums:    make([]int, 0),
		indices: make(map[int]int),
	}
}

func (rs *RandomizedSet) Insert(val int) bool {
	if _, ok := rs.indices[val]; ok {
		return false
	}
	rs.indices[val] = len(rs.nums)
	rs.nums = append(rs.nums, val)

	return true
}

func (rs *RandomizedSet) Remove(val int) bool {
	id, ok := rs.indices[val]
	if !ok {
		return false
	}
	rs.nums[id] = rs.nums[len(rs.nums)-1]
	rs.indices[rs.nums[id]] = id
	rs.nums = rs.nums[:len(rs.nums)-1]
	delete(rs.indices, val)

	return true
}

func (rs *RandomizedSet) GetRandom() int {
	return rs.nums[rand.Intn(len(rs.nums))]
}
