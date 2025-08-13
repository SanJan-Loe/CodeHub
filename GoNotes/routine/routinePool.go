package main

import (
	"net/http"
	_ "net/http/pprof" // 注意：这里是空白导入，只执行init函数
	"sync"
)

type Job func()

type Worker struct {
	id         int
	jobChannel chan Job
	quit       chan bool
}

type Pool struct {
	workers    []*Worker
	jobChannel chan Job
	wg         sync.WaitGroup
}

func NewWorker(id int, jobChannel chan Job) *Worker {
	return &Worker{
		id:         id,
		jobChannel: jobChannel,
		quit:       make(chan bool),
	}
}

func (w *Worker) Start() {
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

func (w *Worker) Stop() {
	go func() {
		w.quit <- true
	}()
}

func NewPool(numWorkers int) *Pool {
	jobChannel := make(chan Job)
	pool := &Pool{
		workers:    make([]*Worker, numWorkers),
		jobChannel: jobChannel,
	}

	for i := 0; i < numWorkers; i++ {
		worker := NewWorker(i, jobChannel)
		pool.workers[i] = worker
	}

	return pool
}

func (p *Pool) Start() {
	for _, worker := range p.workers {
		worker.Start()
	}
}

func (p *Pool) Stop() {
	for _, worker := range p.workers {
		worker.Stop()
	}
}

func (p *Pool) AddJob(job Job) {
	p.wg.Add(1)
	p.jobChannel <- func() {
		job()
		p.wg.Done()
	}
}

func (p *Pool) Wait() {
	p.wg.Wait()
}

func main() {
	// pool := NewPool(5)
	// pool.Start()

	// for i := 0; i < 10; i++ {
	// 	num := i
	// 	pool.AddJob(func() {
	// 		fmt.Printf("Worker %d: %d is odd? %v\n", num%5, num, num%2 == 1)
	// 	})
	// }

	// pool.Wait()
	// pool.Stop()
	myMain()
	// 启动一个HTTP服务器来暴露pprof端点

	// 通常在 localhost 的一个专用端口上监听
	http.ListenAndServe("localhost:6060", nil)
}
