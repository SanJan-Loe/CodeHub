package easy

func productExceptSelf(nums []int) []int {
	left := make([]int, len(nums))
	left[0] = 1
	right := make([]int, len(nums))
	right[len(nums)-1] = 1

	for i := 2; i < len(nums); i++ {
		left[i] = left[i-1] * nums[i-1]
	}

	for i := len(nums) - 2; i >= 0; i-- {
		right[i] = right[i+1] * nums[i+1]
	}

	for i := 0; i < len(nums); i++ {
		nums[i] = left[i] * right[i]
	}
	return nums
}
