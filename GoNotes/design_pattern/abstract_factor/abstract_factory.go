package main

// AbstractFactory 抽象工厂接口
type AbstractFactory interface {
	GetShape(shapeType string) IShape
	GetColor(colorType string) IColor
}