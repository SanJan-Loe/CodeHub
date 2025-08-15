package main

import (
	"fmt"
	"sync"
)

func printCat(wg *sync.WaitGroup, catChan chan struct{}, dogChan chan struct{}, quit chan struct{}) {
	defer wg.Done()

	for i := 0; i < 100; i++ {
		select {
		case <-quit:
			return
		default:
			// 等待信号开始打印
			<-catChan
			fmt.Println("cat")
			// 通知dog可以打印
			dogChan <- struct{}{}
		}
	}
}

func printDog(wg *sync.WaitGroup, dogChan chan struct{}, fishChan chan struct{}, quit chan struct{}) {
	defer wg.Done()

	for i := 0; i < 100; i++ {
		select {
		case <-quit:
			return
		default:
			// 等待cat的信号
			<-dogChan
			fmt.Println("dog")
			// 通知fish可以打印
			fishChan <- struct{}{}
		}
	}
}

func printFish(wg *sync.WaitGroup, fishChan chan struct{}, catChan chan struct{}, quit chan struct{}) {
	defer wg.Done()

	for i := 0; i < 100; i++ {
		select {
		case <-quit:
			return
		default:
			// 等待dog的信号
			<-fishChan
			fmt.Println("fish")
			// 如果不是最后一次，通知cat可以开始下一轮
			if i < 99 {
				catChan <- struct{}{}
			}
		}
	}
}

func PrintInOrder() {
	var wg sync.WaitGroup

	// 创建用于同步的channel
	catChan := make(chan struct{}, 1)
	dogChan := make(chan struct{}, 1)
	fishChan := make(chan struct{}, 1)
	quit := make(chan struct{})

	wg.Add(3)

	// 启动三个goroutine
	go printCat(&wg, catChan, dogChan, quit)
	go printDog(&wg, dogChan, fishChan, quit)
	go printFish(&wg, fishChan, catChan, quit)

	// 开始第一轮打印，通知cat
	catChan <- struct{}{}

	// 等待所有goroutine完成
	wg.Wait()

	fmt.Println("打印完成")
}
