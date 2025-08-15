package main

import (
	"fmt"
	"os"
	"os/signal"
	"syscall"
	"time"
)

func main() {
	// 1. 创建一个用于接收信号的 channel。
	// 使用缓冲为1的channel，确保即使在处理一个信号时，也能接收到另一个信号。
	sigs := make(chan os.Signal, 1)

	// 2. 注册需要监听的信号。
	// syscall.SIGINT: 用户按下 Ctrl+C
	// syscall.SIGTERM: 程序被终止，例如通过 `kill` 命令
	signal.Notify(sigs, syscall.SIGINT, syscall.SIGTERM)

	// 3. 启动一个 goroutine 来等待信号。
	go func() {
		// 从 channel 中阻塞读取信号
		sig := <-sigs
		fmt.Println() // 换行
		fmt.Printf("接收到信号: %s\n", sig)
		fmt.Println("开始执行清理工作...")

		// 在这里执行你的清理逻辑，比如：
		// - 关闭数据库连接
		// - 等待正在处理的 HTTP 请求完成
		// - 保存当前状态到文件
		time.Sleep(2 * time.Second) // 模拟清理工作

		fmt.Println("清理工作完成，程序即将退出。")
		os.Exit(0) // 正常退出
	}()

	fmt.Println("程序已启动，正在运行中...")
	fmt.Println("按下 Ctrl+C 来测试优雅退出。")

	// 主 goroutine 可以继续执行其他任务
	// 这里用一个无限循环来模拟一个长时间运行的服务
	for {
		fmt.Println("服务正在运行...")
		time.Sleep(5 * time.Second)
	}
}
