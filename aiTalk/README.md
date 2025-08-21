# Google Gemini API 客户端

这是一个简单的命令行工具，用于与 Google Gemini API 进行交互。

## 功能特点

- 通过命令行与 Google Gemini API 进行交互
- 支持直接发送单个请求或进入交互式聊天模式
- 可配置的生成参数（温度、top_p、top_k）
- 支持通过环境变量或命令行参数设置 API 密钥
- 完整的错误处理和用户友好的提示

## 安装

1. 克隆或下载此项目
2. 安装所需的依赖项：

```bash
pip install -r requirements.txt
```

## 配置

您可以通过以下两种方式之一设置您的 Google Gemini API 密钥：

### 方法 1：使用 .env 文件

1. 复制 `.env.example` 文件并重命名为 `.env`：

```bash
cp .env.example .env
```

2. 编辑 `.env` 文件，将 `your_gemini_api_key_here` 替换为您的实际 API 密钥：

```
GEMINI_API_KEY=your_actual_api_key_here
```

### 方法 2：使用系统环境变量

在您的 shell 配置文件中（如 `.bashrc`、`.zshrc` 等）添加以下行：

```bash
export GEMINI_API_KEY=your_actual_api_key_here
```

然后重新加载您的 shell 配置文件或重新启动终端。

## 使用方法

### 交互模式

直接运行脚本将进入交互模式：

```bash
python gemini_api.py
```

在交互模式中，您可以输入问题并获取 Gemini 的回答。输入 `quit`、`exit` 或 `退出` 来结束程序。

### 单次请求模式

您也可以通过命令行参数直接发送单个请求：

```bash
python gemini_api.py --prompt "请解释什么是量子计算"
```

### 命令行参数

- `--api-key`: 指定 API 密钥（可选，如果未提供将从环境变量获取）
- `--prompt`: 直接发送的提示（可选，如果未提供将进入交互模式）
- `--temperature`: 控制生成文本的随机性，范围 0.0-1.0（默认: 0.7）
- `--top-p`: 核心采样参数，范围 0.0-1.0（默认: 1.0）
- `--top-k`: 限制词汇选择的数量（默认: 1）

### 示例

```bash
# 使用自定义温度参数
python gemini_api.py --prompt "写一首关于秋天的诗" --temperature 0.9

# 使用自定义 top_p 和 top_k 参数
python gemini_api.py --prompt "解释机器学习的基本概念" --top-p 0.8 --top-k 40

# 直接指定 API 密钥
python gemini_api.py --api-key your_api_key_here --prompt "你好，世界！"
```

## 注意事项

- 确保您有有效的 Google Gemini API 密钥
- API 使用可能会产生费用，请参考 Google 的定价政策
- 此工具需要网络连接才能与 Gemini API 通信

## 错误处理

如果遇到错误，程序会显示友好的错误消息。常见错误包括：

- 未提供 API 密钥
- API 密钥无效
- 网络连接问题
- API 请求限制

## 许可证

此项目采用 MIT 许可证。