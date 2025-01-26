#!/bin/bash

# 检查是否以root权限运行
if [ "$EUID" -ne 0 ]; then 
    echo "Please run as root"
    exit 1
fi



# 设置工作目录为源码目录
WORK_DIR="/usr/local/src/cqchat_server/verify_server"

CONFIG_DIR="/usr/local/etc/cqchat_server"
LOG_DIR="/usr/local/var/logs/cqchat_server/verify_server"

# 检查工作目录
if [ ! -d "$WORK_DIR" ]; then
    echo "Working directory not found: $WORK_DIR"
    exit 1
fi

# 检查并创建日志目录
if [ ! -d "$LOG_DIR" ]; then
    mkdir -p "$LOG_DIR"
    chown chenqi:chenqi "$LOG_DIR"
fi

# 切换到工作目录
cd "$WORK_DIR"

# 检查 node_modules
if [ ! -d "$WORK_DIR/node_modules" ]; then
    echo "Installing dependencies..."
    npm install
fi

# 启动服务
cd "$WORK_DIR"
exec /usr/bin/node server.js 