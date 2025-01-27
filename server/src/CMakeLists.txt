# 查找公共依赖包
find_package(Threads REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(jsoncpp REQUIRED)
find_package(Boost REQUIRED COMPONENTS system filesystem)
# find_package(mysqlcppconn REQUIRED)
#find_package(Hiredis REQUIRED)  通过cmake构建的项目，find_package才能找到，虽然是安装在标准目录下
#find_package(OpenSSL REQUIRED)


# 替换为手动查找 hiredis 库
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


# 添加调试信息，看看变量的值
message(STATUS "Checking HIREDIS_LIB: ${HIREDIS_LIBRARY}")
message(STATUS "Checking HIREDIS_INCLUDE: ${HIREDIS_INCLUDE_DIR}")


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
add_subdirectory(gate_server)










