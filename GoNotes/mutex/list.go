package main

import "sync"

type SafeArray struct {
	mu   sync.Mutex
	data []interface{}
}

func NewSafeArray() *SafeArray {
	return &SafeArray{
		data: make([]interface{}, 0),
	}
}

func (sa *SafeArray) Append(item interface{}) {
	sa.mu.Lock()
	defer sa.mu.Unlock()
	sa.data = append(sa.data, item)
}

func (sa *SafeArray) Get(index int) (interface{}, bool) {
	sa.mu.Lock()
	defer sa.mu.Unlock()

	if index < 0 || index >= len(sa.data) {
		return nil, false
	}

	return sa.data[index], true
}

func (sa *SafeArray) Len() int {
	sa.mu.Lock()
	defer sa.mu.Unlock()
	return len(sa.data)
}

func (sa *SafeArray) Remove(index int) bool {
	sa.mu.Lock()
	defer sa.mu.Unlock()

	if index < 0 || index >= len(sa.data) {
		return false
	}

	// 删除元素，使用切片操作来移除
	sa.data = append(sa.data[:index], sa.data[index+1:]...)
	return true
}
