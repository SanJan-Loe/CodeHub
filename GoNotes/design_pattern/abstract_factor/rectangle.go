package main

// Rectangle 矩形结构体
type Rectangle struct{}

// Draw 实现形状接口的Draw方法
func (r *Rectangle) Draw() string {
	return "绘制矩形"
}