package main

// ShapeFactory 形状工厂
type ShapeFactory struct{}

// GetShape 根据类型获取形状
func (s *ShapeFactory) GetShape(shapeType string) IShape {
	if shapeType == "" {
		return nil
	}
	
	switch shapeType {
	case "CIRCLE":
		return &Circle{}
	case "RECTANGLE":
		return &Rectangle{}
	default:
		return nil
	}
}

// GetColor 形状工厂不支持获取颜色
func (s *ShapeFactory) GetColor(colorType string) IColor {
	return nil
}