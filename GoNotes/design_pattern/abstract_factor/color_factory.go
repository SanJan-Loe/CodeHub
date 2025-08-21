package main

// ColorFactory 颜色工厂
type ColorFactory struct{}

// GetShape 颜色工厂不支持获取形状
func (c *ColorFactory) GetShape(shapeType string) IShape {
	return nil
}

// GetColor 根据类型获取颜色
func (c *ColorFactory) GetColor(colorType string) IColor {
	if colorType == "" {
		return nil
	}
	
	switch colorType {
	case "RED":
		return &Red{}
	case "BLUE":
		return &Blue{}
	default:
		return nil
	}
}