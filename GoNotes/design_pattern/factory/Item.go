package main

type Item struct {
	Name     string
	Price    int
	Quantity int
}

func (i *Item) GetName() string {
	return i.Name
}

func (i *Item) GetPrice() int {
	return i.Price
}

func (i *Item) GetQuantity() int {
	return i.Quantity
}

func (i *Item) SetQuantity(quantity int) {
	i.Quantity = quantity
}

func (i *Item) SetPrice(price int) {
	i.Price = price
}

func (i *Item) SetName(name string) {
	i.Name = name
}
