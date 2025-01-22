#!/bin/bash

# 使用 systemd service 文件中定义的环境变量，如果没有则使用默认值（与安装路径保持一致）
CONFIG_FILE="/etc/cqchat_server/config.ini"
LOG_DIR="/usr/local/logs/cqchat_server"

# 解析命令行参数，允许用户自定义配置文件和日志目录
# 例如: ./start.sh --config=/custom/path/config.ini --log-dir=/custom/logs
while [[ $# -gt 0 ]]; do               # 当还有命令行参数时继续循环
    case $1 in
        --config=*)                     # 如果参数以 --config= 开头
            CONFIG_FILE="${1#*=}"       # 获取等号后面的值作为配置文件路径
            shift                       # 移动到下一个参数
            ;;
        --log-dir=*)                    # 如果参数以 --log-dir= 开头
            LOG_DIR="${1#*=}"           # 获取等号后面的值作为日志目录
            shift                       # 移动到下一个参数
            ;;
        *)                              # 未知参数
            echo "Unknown parameter: $1" # 输出错误信息
            exit 1                      # 退出程序
            ;;
    esac
done

# 检查配置文件是否存在
if [ ! -f "$CONFIG_FILE" ]; then
    echo "Config file not found: $CONFIG_FILE"
    exit 1
fi

# 检查并创建日志目录（如果不存在）
if [ ! -d "$LOG_DIR" ]; then
    mkdir -p "$LOG_DIR"
    # 设置目录权限（如果以root运行）
    if [ $(id -u) -eq 0 ]; then
        chown cqchat_server:cqchat_server "$LOG_DIR"
        chmod 755 "$LOG_DIR"
    fi
fi

# 设置环境变量，让程序知道配置文件和日志目录的位置
export CQCHAT_SERVER_CONFIG="$CONFIG_FILE"
export CQCHAT_SERVER_LOG_DIR="$LOG_DIR"

# 启动服务器程序
# exec 会替换当前 shell 进程，而不是创建子进程
# 这样可以正确传递信号（如 SIGTERM）给服务器程序
exec /usr/local/bin/cqchat_server