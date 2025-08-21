package main

// Circle 圆形结构体
type Circle struct{}

// Draw 实现形状接口的Draw方法
func (c *Circle) Draw() string {
	return "绘制圆形"
}