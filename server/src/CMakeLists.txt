# 查找公共依赖包
find_package(Threads REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(jsoncpp REQUIRED)
find_package(Boost REQUIRED COMPONENTS system filesystem)
# find_package(mysqlcppconn REQUIRED)
#find_package(Hiredis REQUIRED)  通过cmake构建的项目，find_package才能找到，虽然是安装在标准目录下
#find_package(OpenSSL REQUIRED)


# 替换为手动查找 hiredis 和 mysqlcppconn 库
#find_library(<VAR> name PATHS ... OPTIONS ...) 
find_library(HIREDIS_LIBRARY NAMES libhiredis.so)     
find_library(MYSQLCPPCONN_LIBRARY NAMES mysqlcppconn)
#find_path(<VAR> name PATHS ... OPTIONS ...) PATHS可以省略，默认是标准安装目录
find_path(HIREDIS_INCLUDE_DIR NAMES hiredis/hiredis.h)
find_path(MYSQLCPPCONN_INCLUDE_DIR 
        NAMES 
        mysql_connection.h 
        mysql_driver.h
        cppconn/driver.h
        cppconn/exception.h
        cppconn/resultset.h
        cppconn/statement.h
        cppconn/prepared_statement.h
        )

# 设置公共包含目录
set(COMMON_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Protobuf_INCLUDE_DIRS}
    ${gRPC_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    CACHE INTERNAL "Common include directories"
)

# 设置基础公共库（不包含 common_lib）
set(COMMON_LIBRARIES
    gRPC::grpc
    gRPC::grpc++
    gRPC::grpc++_reflection
    jsoncpp
    Threads::Threads
    boost_system
    boost_filesystem
    CACHE INTERNAL "Common libraries"
)


# 检查并添加 hiredis
if(HIREDIS_LIBRARY AND HIREDIS_INCLUDE_DIR)
    message(STATUS "Found hiredis: ${HIREDIS_LIBRARY}")
    list(APPEND COMMON_INCLUDE_DIRS ${HIREDIS_INCLUDE_DIR})
    list(APPEND COMMON_LIBRARIES ${HIREDIS_LIBRARY})
else()
    message(FATAL_ERROR "hiredis not found!")
endif()


# 检查并添加 mysqlcppconn
if(MYSQLCPPCONN_LIBRARY AND MYSQLCPPCONN_INCLUDE_DIR)
    message(STATUS "Found mysqlcppconn: ${MYSQLCPPCONN_LIBRARY}")
    list(APPEND COMMON_INCLUDE_DIRS ${MYSQLCPPCONN_INCLUDE_DIR})
    list(APPEND COMMON_LIBRARIES ${MYSQLCPPCONN_LIBRARY})
else()
    message(FATAL_ERROR "mysqlcppconn library not found")
endif()

# 添加调试信息，看看变量的值
# message(STATUS "Checking HIREDIS_LIB: ${HIREDIS_LIBRARY}")
# message(STATUS "Checking HIREDIS_INCLUDE: ${HIREDIS_INCLUDE_DIR}")


# 添加子目录
# add_subdirectory(VarifyServer)
add_subdirectory(gate_server)
add_subdirectory(common)
add_subdirectory(status_server)
add_subdirectory(chat_server)









