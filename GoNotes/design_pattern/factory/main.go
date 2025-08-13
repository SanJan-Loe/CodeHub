package main

import "fmt"

func main() {

	apple, err := newItem("apple")
	if err != nil {
		fmt.Println(err)
	}

	fmt.Printf("here get the apple:%v\n", apple)

	pie, err := newItem("pie")
	if err != nil {
		fmt.Println(err)
	}

	fmt.Printf("here get the pie:%v\n", pie)

}
