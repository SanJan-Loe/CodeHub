package logic

import (
	"context"
	"encoding/json"
	"fmt"
	"time"

	"codeHub/gozero/user/internal/svc"
	"codeHub/gozero/user/internal/types"

	"github.com/segmentio/kafka-go"
	"github.com/zeromicro/go-zero/core/logx"
)

type LoginLogic struct {
	logx.Logger
	ctx    context.Context
	svcCtx *svc.ServiceContext
}

func NewLoginLogic(ctx context.Context, svcCtx *svc.ServiceContext) *LoginLogic {
	return &LoginLogic{
		Logger: logx.WithContext(ctx),
		ctx:    ctx,
		svcCtx: svcCtx,
	}
}

func (l *LoginLogic) Login(req *types.LoginRequest) (resp *types.LoginResponse, err error) {
	// 登录逻辑处理
	// 这里可以添加用户验证逻辑，例如检查用户名和密码
	
	// 创建登录事件消息
	loginEvent := map[string]interface{}{
		"username":   req.Username,
		"password":   req.Password, // 实际应用中不应该传输明文密码
		"timestamp":  fmt.Sprintf("%d", time.Now().Unix()),
		"event_type": "user_login",
	}
	
	// 将事件转换为 JSON
	message, err := json.Marshal(loginEvent)
	if err != nil {
		l.Errorf("Failed to marshal login event: %v", err)
		return nil, err
	}
	
	// 发送登录事件到 Kafka
	err = l.svcCtx.KafkaWriter.WriteMessages(l.ctx, kafka.Message{
		Key:   []byte(req.Username),
		Value: message,
	})
	if err != nil {
		l.Errorf("Failed to send login event to Kafka: %v", err)
		return nil, err
	}
	
	l.Infof("Login event sent to Kafka for user: %s", req.Username)
	
	// 返回登录响应
	// 这里应该是实际的用户验证逻辑，生成 token 等
	// 现在只是示例代码
	currentTime := time.Now()
	resp = &types.LoginResponse{
		Token:     "example-token-" + req.Username,
		ExpiresAt: currentTime.Add(24 * time.Hour).Unix(),
		UserInfo: types.UserInfo{
			Id:       1, // 应该从数据库获取
			Username: req.Username,
			Email:    req.Username + "@example.com",
			CreateAt: currentTime.Format("2006-01-02 15:04:05"),
			UpdateAt: currentTime.Format("2006-01-02 15:04:05"),
		},
	}
	
	return resp, nil
}
