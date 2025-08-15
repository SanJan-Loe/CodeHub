package main

import (
	"fmt"
	"sync"
)

type Data struct {
	Id   int
	Info string
}

func main() {
	// 创建一个带缓冲的channel, 缓冲大小为10
	dataChannel := make(chan Data)

	// 使用WaitGroup来等待所有的协程完成
	var wg sync.WaitGroup

	wg.Add(1)

	go func() {
		for i := 0; i < 10; i++ {
			data := <-dataChannel
			fmt.Printf("here get the data:%v\n", data)
		}
		wg.Done()
	}()

	for i := 0; i < 10; i++ {
		dataChannel <- Data{
			Id:   i,
			Info: fmt.Sprintf("data:%d", i),
		}
	}
	wg.Wait()
}
