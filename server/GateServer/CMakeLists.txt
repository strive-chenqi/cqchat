# cmake_minimum_required(VERSION 3.14)

# project(CServer)

# # 设置 C++ 标准
# set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_CXX_STANDARD_REQUIRED True)

# # 查找 Protobuf 和 gRPC
# find_package(Protobuf CONFIG REQUIRED)
# find_package(gRPC CONFIG REQUIRED)
# find_package(Threads REQUIRED)

# # 手动设置 hiredis、MySQL Connector/C++ 的包含目录和库目录
# set(HIREDIS_INCLUDE_DIR /usr/include/hiredis)
# set(HIREDIS_LIB_DIR /usr/local/lib)
# set(MYSQL_INCLUDE_DIR /usr/include)
# set(MYSQL_LIB_DIR /usr/lib/x86_64-linux-gnu)

# # 添加包含目录  
# include_directories(${HIREDIS_INCLUDE_DIR} ${MySQL_INCLUDE_DIRS})

# # 添加库目录
# link_directories(${HIREDIS_LIB_DIR} ${MySQL_LIBRARY_DIRS})

# # 设置 Protobuf 和 gRPC 相关变量
# set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
# set(_GRPC_GRPCPP gRPC::grpc++)

# # 包含项目源文件
# set(SRCS
#   CServer.cpp
#   HttpConnection.cpp
#   LogicSystem.cpp
#   GateServer.cpp
#   ConfigMgr.cpp
#   VerifyGrpcClient.cpp
#   AsioIOServicePool.cpp
#   RedisMgr.cpp
#   MysqlDao.cpp
#   MysqlMgr.cpp
#   message.pb.cc
#   message.grpc.pb.cc
# )

# # 添加可执行文件
# add_executable(${PROJECT_NAME} ${SRCS})

# # 链接库
# target_link_libraries(${PROJECT_NAME}
#   ${_PROTOBUF_LIBPROTOBUF}
#   ${_GRPC_GRPCPP}
#   gRPC::grpc
#   gRPC::grpc++
#   gRPC::grpc++_reflection
#   jsoncpp
#   Threads::Threads
#   boost_system
#   boost_filesystem
#   ssl
#   crypto
#   hiredis
#   mysqlcppconn
# )

# # 包含目录
# target_include_directories(${PROJECT_NAME} PRIVATE ${Protobuf_INCLUDE_DIRS} ${gRPC_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})

# # 添加编译选项
# target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)