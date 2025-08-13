package main

type Pie struct {
	Item
}

func newPie() IItem {
	return &Pie{
		Item: Item{
			Name:     "pie",
			Price:    10,
			Quantity: 5,
		},
	}
}
