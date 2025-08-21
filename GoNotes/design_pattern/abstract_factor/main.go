package main

import "fmt"

func main() {
	// 创建工厂生成器
	producer := &FactoryProducer{}
	
	fmt.Println("=== 抽象工厂模式演示 ===")
	
	// 获取形状工厂
	shapeFactory := producer.GetFactory("SHAPE")
	if shapeFactory != nil {
		// 获取圆形对象
		circle := shapeFactory.GetShape("CIRCLE")
		if circle != nil {
			fmt.Println(circle.Draw())
		}
		
		// 获取矩形对象
		rectangle := shapeFactory.GetShape("RECTANGLE")
		if rectangle != nil {
			fmt.Println(rectangle.Draw())
		}
	}
	
	fmt.Println()
	
	// 获取颜色工厂
	colorFactory := producer.GetFactory("COLOR")
	if colorFactory != nil {
		// 获取红色对象
		red := colorFactory.GetColor("RED")
		if red != nil {
			fmt.Println(red.Fill())
		}
		
		// 获取蓝色对象
		blue := colorFactory.GetColor("BLUE")
		if blue != nil {
			fmt.Println(blue.Fill())
		}
	}
	
	fmt.Println()
	fmt.Println("演示完成！")
}