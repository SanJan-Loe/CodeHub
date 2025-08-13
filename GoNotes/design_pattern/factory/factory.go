package main

import "errors"

func newItem(name string) (IItem, error) {
	if name == "apple" {
		return &Apple{
			Item: Item{
				Name:     "apple",
				Price:    5,
				Quantity: 10,
			},
		}, nil
	}

	if name == "pie" {
		return &Pie{
			Item: Item{
				Name:     "pie",
				Price:    10,
				Quantity: 5,
			},
		}, nil
	}

	return nil, errors.New("no such item")
}
