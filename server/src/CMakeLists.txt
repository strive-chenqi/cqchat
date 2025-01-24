# 查找公共依赖包
find_package(Threads REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(jsoncpp REQUIRED)
find_package(Boost REQUIRED COMPONENTS system filesystem)
#find_package(OpenSSL REQUIRED)

# 设置公共包含目录
# set(COMMON_INCLUDE_DIRS
#     ${CMAKE_CURRENT_SOURCE_DIR}
#     ${Protobuf_INCLUDE_DIRS}
#     ${GRPC_INCLUDE_DIRS}
#     ${Boost_INCLUDE_DIRS}
# #    ${OPENSSL_INCLUDE_DIR}
# )

# 添加子目录
# add_subdirectory(VarifyServer)
add_subdirectory(GateServer)










