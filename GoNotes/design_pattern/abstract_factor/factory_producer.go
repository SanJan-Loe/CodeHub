package main

// FactoryProducer 工厂生成器
type FactoryProducer struct{}

// GetFactory 根据类型获取工厂
func (f *FactoryProducer) GetFactory(choice string) AbstractFactory {
	if choice == "" {
		return nil
	}
	
	switch choice {
	case "SHAPE":
		return &ShapeFactory{}
	case "COLOR":
		return &ColorFactory{}
	default:
		return nil
	}
}