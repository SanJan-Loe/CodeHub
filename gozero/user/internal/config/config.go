package config

import (
	"github.com/zeromicro/go-zero/rest"
)

type KafkaConfig struct {
	Brokers []string
	Topic   string
	GroupId string
}

type Config struct {
	rest.RestConf
	KafkaConf KafkaConfig
}
