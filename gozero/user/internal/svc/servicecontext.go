package svc

import (
	"codeHub/gozero/user/internal/config"
	
	"github.com/segmentio/kafka-go"
	"github.com/zeromicro/go-zero/core/logx"
)

type ServiceContext struct {
	Config      config.Config
	KafkaWriter *kafka.Writer
	KafkaReader *kafka.Reader
}

func NewServiceContext(c config.Config) *ServiceContext {
	// 创建 Kafka 生产者
	kafkaWriter := kafka.NewWriter(kafka.WriterConfig{
		Brokers:  c.KafkaConf.Brokers,
		Topic:    c.KafkaConf.Topic,
		Balancer: &kafka.LeastBytes{},
	})
	
	// 创建 Kafka 消费者
	kafkaReader := kafka.NewReader(kafka.ReaderConfig{
		Brokers:  c.KafkaConf.Brokers,
		Topic:    c.KafkaConf.Topic,
		GroupID:  c.KafkaConf.GroupId,
		MinBytes: 10e3, // 10KB
		MaxBytes: 10e6, // 10MB
	})
	
	logx.Info("Kafka client initialized")
	
	return &ServiceContext{
		Config:      c,
		KafkaWriter: kafkaWriter,
		KafkaReader: kafkaReader,
	}
}
