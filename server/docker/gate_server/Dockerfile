# 构建阶段
FROM ubuntu:22.04 AS builder

# 设置工作目录，设置的是在 Docker 镜像/容器中的工作目录。
WORKDIR /cqchat_server

# 安装必要的包和依赖
RUN apt-get update && apt-get install -y \
    build-essential \
    libjsoncpp-dev \
    cmake \
    wget \
    pkg-config \
    libmysqlcppconn-dev \
    && rm -rf /var/lib/apt/lists/*

# 安装 gRPC
RUN git clone -b v1.34.0 https://gitee.com/mirrors/grpc-framework.git grpc && \
    cd grpc && \
    git submodule update --init && \
    mkdir build && \
    cd build && \
    cmake -DCMAKE_INSTALL_PREFIX=/usr/local .. && \
    \
    sed -i '437i\  using namespace std;' ../third_party/abseil-cpp/absl/synchronization/internal/graphcycles.cc && \
    sed -i '42a\  #include <limits>' ../third_party/abseil-cpp/absl/synchronization/internal/graphcycles.cc && \
    sed -i '20a\  #include <stdint.h>' ../third_party/abseil-cpp/absl/strings/internal/str_format/extension.h && \
    sed -i '138 s/size_t stack_size = (std::max(SIGSTKSZ, 65536) + page_mask) \& ~page_mask;/size_t stack_size = (std::max<unsigned long>(SIGSTKSZ, 65536) + page_mask) \& ~page_mask;/' ../third_party/abseil-cpp/absl/debugging/failure_signal_handler.cc && \
    \
    make -j4 && \
    make install


# 安装 hiredis
RUN git clone https://github.com/redis/hiredis.git && \
    cd hiredis && \
    make && \
    make install && \
    ldconfig

# 安装 boost
RUN wget https://archives.boost.io/release/1.87.0/source/boost_1_87_0.tar.gz && \
    tar xzf boost_1_87_0.tar.gz && \
    cd boost_1_87_0 && \
    ./bootstrap.sh && \
    ./b2 install


# 复制项目文件 COPY <src> <dest>
# 源路径：Dockerfile 所在目录的所有文件和文件夹,目标路径：当前工作目录（WORKDIR /cqchat_server）
COPY .. .

# 构建项目
RUN cd build \
    && cmake .. \
    && make


    
# 运行阶段
FROM ubuntu:22.04

WORKDIR /cqchat_server/run

# 安装运行时依赖
RUN apt-get update && apt-get install -y \
    libstdc++6 \
    nodejs \
    npm \
    && rm -rf /var/lib/apt/lists/*

# 从构建阶段复制编译结果
COPY --from=builder /usr/local/lib /usr/local/lib
COPY --from=builder /usr/local/include /usr/local/include
COPY --from=builder /cqchat_server/bin/cqchat_server ./
RUN ldconfig

# 暴露需要的端口
EXPOSE 3306 6379 8080

# 启动命令
CMD ["./gate_server"]