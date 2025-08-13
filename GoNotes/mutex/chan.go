package main

import "sync"

type Data struct {
	Id   int
	Info string
}

func main() {
	// 创建一个带缓冲的channel, 缓冲大小为10
	dataChannel := make(chan Data, 10)

	// 使用WaitGroup来等待所有的协程完成
	var wg sync.WaitGroup

	wg.Add(numPro)
}
