package main

// Red 红色结构体
type Red struct{}

// Fill 实现颜色接口的Fill方法
func (r *Red) Fill() string {
	return "填充红色"
}