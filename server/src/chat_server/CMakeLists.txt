project(chat_server)

aux_source_directory(. SRCS)

# 添加可执行文件
add_executable(${PROJECT_NAME} ${SRCS})

# 包含目录
target_include_directories(${PROJECT_NAME} 
    PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${INCLUDE_DIRS}
)

# 链接库
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    ${COMMON_LIBRARIES}
    common_lib
)

