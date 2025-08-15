package main

import "fmt"

func main() {
	s := make([]int, 511, 512)
	fmt.Printf("初始: len=%d, cap=%d, ptr=%p\n", len(s), cap(s), s)

	// 第1次 append，容量足够
	s = append(s, 1)
	fmt.Printf("Append 1: len=%d, cap=%d, ptr=%p\n", len(s), cap(s), s)

	// 第2次 append，容量不足，触发扩容
	s = append(s, s[1:10]...)
	fmt.Printf("Append 2: len=%d, cap=%d, ptr=%p\n", len(s), cap(s), s)

	// 第3次 append
	s = append(s, 3)
	fmt.Printf("Append 3: len=%d, cap=%d, ptr=%p\n", len(s), cap(s), s)
}
