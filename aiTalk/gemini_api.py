#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Google Gemini API 客户端
用于与 Google Gemini API 进行交互的命令行工具
"""

import os
import sys
import argparse
import google.generativeai as genai
from dotenv import load_dotenv

# 加载环境变量
load_dotenv()


class GeminiClient:
    """Google Gemini API 客户端类"""
    
    def __init__(self, api_key=None):
        """
        初始化 Gemini 客户端
        
        Args:
            api_key (str, optional): Gemini API 密钥。如果未提供，将从环境变量中获取。
        """
        self.api_key = api_key or os.getenv('GEMINI_API_KEY')
        if not self.api_key:
            raise ValueError("未提供 API 密钥。请设置 GEMINI_API_KEY 环境变量或在初始化时提供密钥。")
        
        # 配置 Gemini API
        genai.configure(api_key=self.api_key)
        
        # 创建模型实例
        self.model = genai.GenerativeModel('gemini-pro')
    
    def send_request(self, prompt, temperature=0.7, top_p=1.0, top_k=1):
        """
        向 Gemini API 发送请求
        
        Args:
            prompt (str): 用户输入的提示
            temperature (float): 控制生成文本的随机性，范围 0.0-1.0
            top_p (float): 核心采样参数，范围 0.0-1.0
            top_k (int): 限制词汇选择的数量
            
        Returns:
            str: Gemini API 的响应文本
            
        Raises:
            Exception: 当 API 请求失败时抛出异常
        """
        try:
            # 设置生成配置
            generation_config = {
                "temperature": temperature,
                "top_p": top_p,
                "top_k": top_k,
                "max_output_tokens": 2048,
            }
            
            # 发送请求并获取响应
            response = self.model.generate_content(
                prompt,
                generation_config=generation_config
            )
            
            return response.text
            
        except Exception as e:
            raise Exception(f"请求 Gemini API 时出错: {str(e)}")
    
    def chat_loop(self):
        """启动交互式聊天循环"""
        print("欢迎使用 Google Gemini API 客户端！")
        print("输入 'quit' 或 'exit' 退出程序。")
        print("----------------------------------")
        
        while True:
            try:
                # 获取用户输入
                user_input = input("\n请输入您的问题: ")
                
                # 检查退出命令
                if user_input.lower() in ['quit', 'exit', '退出']:
                    print("感谢使用，再见！")
                    break
                
                # 发送请求并获取响应
                response = self.send_request(user_input)
                
                # 打印响应
                print("\nGemini 回复:")
                print(response)
                
            except KeyboardInterrupt:
                print("\n检测到中断信号，程序退出。")
                break
            except Exception as e:
                print(f"\n错误: {str(e)}")


def main():
    """主函数，处理命令行参数并启动程序"""
    # 设置命令行参数解析
    parser = argparse.ArgumentParser(description='Google Gemini API 客户端')
    parser.add_argument(
        '--api-key', 
        type=str, 
        help='Google Gemini API 密钥（可选，如果未提供将从环境变量获取）'
    )
    parser.add_argument(
        '--prompt', 
        type=str, 
        help='直接发送的提示（可选，如果未提供将进入交互模式）'
    )
    parser.add_argument(
        '--temperature', 
        type=float, 
        default=0.7, 
        help='控制生成文本的随机性，范围 0.0-1.0（默认: 0.7）'
    )
    parser.add_argument(
        '--top-p', 
        type=float, 
        default=1.0, 
        help='核心采样参数，范围 0.0-1.0（默认: 1.0）'
    )
    parser.add_argument(
        '--top-k', 
        type=int, 
        default=1, 
        help='限制词汇选择的数量（默认: 1）'
    )
    
    # 解析命令行参数
    args = parser.parse_args()
    
    try:
        # 创建 Gemini 客户端
        client = GeminiClient(api_key=args.api_key)
        
        # 如果提供了直接提示，则发送请求并打印结果
        if args.prompt:
            response = client.send_request(
                args.prompt, 
                temperature=args.temperature,
                top_p=args.top_p,
                top_k=args.top_k
            )
            print(response)
        else:
            # 否则进入交互模式
            client.chat_loop()
            
    except ValueError as e:
        print(f"配置错误: {str(e)}", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"运行时错误: {str(e)}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()