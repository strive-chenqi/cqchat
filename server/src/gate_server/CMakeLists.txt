project(gate_server)

aux_source_directory(. SRCS)

# 添加可执行文件
add_executable(${PROJECT_NAME} ${SRCS})


# 设置头文件目录
set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Protobuf_INCLUDE_DIRS}
    ${gRPC_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${HIREDIS_INCLUDE_DIR}
    #${OpenSSL_INCLUDE_DIRS}

)


if(HIREDIS_LIBRARY AND HIREDIS_INCLUDE_DIR)
    #message(STATUS "Found hiredis: ${HIREDIS_LIBRARY}, ${HIREDIS_INCLUDE_DIR}")
    target_include_directories(${PROJECT_NAME} PRIVATE ${HIREDIS_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} ${HIREDIS_LIBRARY})
else()
    message(FATAL_ERROR "hiredis not found!")
endif()


if(MYSQLCPPCONN_LIBRARY AND MYSQLCPPCONN_INCLUDE_DIR)
    message(STATUS "Found mysqlcppconn: ${MYSQLCPPCONN_LIBRARY}, ${MYSQLCPPCONN_INCLUDE_DIR}")
    target_include_directories(${PROJECT_NAME} PRIVATE ${MYSQLCPPCONN_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} ${MYSQLCPPCONN_LIBRARY})
else()
    message(FATAL_ERROR "mysqlcppconn library not found")
endif()

# 设置库文件目录 
# set(LIB_DIRS
#     ${Protobuf_LIBRARY_DIRS}
#     ${gRPC_LIBRARY_DIRS}
# )


target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})
#当使用 find_package 找到包时，包会自动提供正确的库路径，如果是非标准位置的库可以使用
#target_link_directories(${PROJECT_NAME} PRIVATE ${LIB_DIRS})

target_link_libraries(${PROJECT_NAME}
  gRPC::grpc
  gRPC::grpc++
  gRPC::grpc++_reflection
  jsoncpp
  Threads::Threads
  boost_system
  boost_filesystem
  hiredis
  mysqlcppconn

  #varify_proto   # 链接proto库，它会自动包含必要的头文件
  
  # OpenSSL::SSL
  # OpenSSL::Crypto
)









