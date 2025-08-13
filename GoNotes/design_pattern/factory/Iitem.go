package main

type IItem interface {
	GetName() string
	GetPrice() int
	GetQuantity() int
	SetQuantity(int)
	SetPrice(int)
	SetName(string)
}
