# 设置项目名称
project(status_server)

aux_source_directory(. SRCS)

# 添加可执行文件
add_executable(${PROJECT_NAME} ${SRCS})

# 设置包含目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ${COMMON_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 链接依赖库
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${COMMON_LIBRARIES}
    common_lib
)