package main

type Apple struct {
	Item
}

func newApple() IItem {
	return &Apple{
		Item: Item{
			Name:     "apple",
			Price:    5,
			Quantity: 10,
		},
	}
}
