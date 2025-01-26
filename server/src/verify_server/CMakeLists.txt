# project(VarifyServer)


# # 生成 protobuf 和 grpc 文件
# # add_custom_command(
# #     OUTPUT 
# #         ${CMAKE_CURRENT_BINARY_DIR}/message.pb.cc
# #         ${CMAKE_CURRENT_BINARY_DIR}/message.pb.h
# #         ${CMAKE_CURRENT_BINARY_DIR}/message.grpc.pb.cc
# #         ${CMAKE_CURRENT_BINARY_DIR}/message.grpc.pb.h
# #     COMMAND protoc
# #         --grpc_out=${CMAKE_CURRENT_BINARY_DIR}
# #         --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
# #         --proto_path=${CMAKE_CURRENT_SOURCE_DIR}
# #         --plugin=protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>
# #         ${CMAKE_CURRENT_SOURCE_DIR}/message.proto
# #     DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/message.proto
# # )

# # 源文件列表
# set(SRCS
#     message.pb.cc
#     message.grpc.pb.cc
#     # 添加其他源文件
# )

# #对于跨目录依赖的处理 1、创建接口库方案（推荐） 
# # 创建库
# add_library(varify_proto STATIC ${SRCS})

# # 设置包含目录
# target_include_directories(varify_proto PUBLIC
#     #${COMMON_INCLUDE_DIRS}
#     ${CMAKE_CURRENT_SOURCE_DIR} # 让其他项目能找到头文件
# )

# # 链接库
# target_link_libraries(varify_proto PUBLIC
#     protobuf::libprotobuf
#     gRPC::grpc++
#     gRPC::grpc++_reflection
# )

