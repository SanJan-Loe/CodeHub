package main

import (
	"fmt"
	"sync"
)

type my_Job func()

type my_Worker struct {
	id         int
	jobChannel chan my_Job
	quit       chan bool
}

type my_Pool struct {
	worker     []*my_Worker
	jobChannel chan my_Job
	wg         sync.WaitGroup
}

func NewMyWorker(id int, jobChannel chan my_Job) *my_Worker {
	return &my_Worker{
		id:         id,
		jobChannel: jobChannel,
		quit:       make(chan bool),
	}
}

func (w *my_Worker) Start() {
	go func() {
		for {
			select {
			case job := <-w.jobChannel:
				job()
			case <-w.quit:
				return
			}
		}
	}()
}

func (w *my_Worker) Stop() {
	go func() {
		w.quit <- true
	}()
}

func NewMyPool(numWorkers int) *my_Pool {
	jobChannel := make(chan my_Job)
	pool := &my_Pool{
		worker:     make([]*my_Worker, numWorkers),
		jobChannel: jobChannel,
	}

	for i := 0; i < numWorkers; i++ {
		worker := NewMyWorker(i, jobChannel)
		pool.worker[i] = worker
	}

	return pool
}

func (p *my_Pool) Start() {
	for _, worker := range p.worker {
		worker.Start()
	}
}

func (p *my_Pool) Stop() {
	for _, worker := range p.worker {
		worker.Stop()
	}
}

func (p *my_Pool) AddJob(job my_Job) {
	p.wg.Add(1)
	p.jobChannel <- func() {
		job()
		p.wg.Done()
	}
}

func (p *my_Pool) Wait() {
	p.wg.Wait()
}

func myMain() {
	pool := NewMyPool(5)
	pool.Start()

	for i := 0; i < 10; i++ {
		num := i
		pool.AddJob(func() {
			fmt.Printf("Worker %d: %d is odd? %v\n", num%5, num, num%2 == 1)
		})
	}

	pool.Wait()
	pool.Stop()
}
