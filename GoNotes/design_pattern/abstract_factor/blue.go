package main

// Blue 蓝色结构体
type Blue struct{}

// Fill 实现颜色接口的Fill方法
func (b *Blue) Fill() string {
	return "填充蓝色"
}