package main

import (
	"fmt"
	"runtime"
	"time"
)

func main() {
	// 模拟一些内存分配
	for i := 0; i < 1000000; i++ {
		_ = make([]byte, 1024)
	}

	fmt.Println("Before manual GC")

	// 手动触发 GC
	runtime.GC()

	fmt.Println("After manual GC")
	time.Sleep(1 * time.Second) // 等待 GC 完成
}
