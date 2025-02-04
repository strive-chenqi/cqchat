# 设置生成文件的路径
set(PROTO_GENERATED_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/message.pb.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/message.pb.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/message.grpc.pb.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/message.grpc.pb.h"
)

# 默认需要生成
set(NEED_GENERATE FALSE)

# 遍历检查所有生成的文件是否都存在
foreach(GENERATED_FILE ${PROTO_GENERATED_FILES})
    if(NOT EXISTS ${GENERATED_FILE})
        set(NEED_GENERATE TRUE)
        break()
    endif()
endforeach()

# 只在需要时生成
if(NEED_GENERATE)
    message(STATUS "Generating protobuf files...")
    # 生成proto源文件命令
    add_custom_command(
        OUTPUT ${PROTO_GENERATED_FILES}
        COMMAND ${PROTOBUF_PROTOC_EXECUTABLE}
            --grpc_out="${CMAKE_CURRENT_BINARY_DIR}"
            --cpp_out="${CMAKE_CURRENT_BINARY_DIR}"
            -I="${CMAKE_CURRENT_SOURCE_DIR}"
            --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
            ${PROTO_FILES}
        DEPENDS ${PROTO_FILES}
        COMMENT "Generating protobuf and gRPC files"
    )
endif()

aux_source_directory(. SRCS)

# 创建common库
add_library(common_lib STATIC
    ${PROTO_GENERATED_FILES}
    ${SRCS}

)

# 设置包含目录
target_include_directories(common_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${COMMON_INCLUDE_DIRS}
)


# 链接依赖
target_link_libraries(common_lib PUBLIC
    ${COMMON_LIBRARIES}
)
