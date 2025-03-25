#file(GLOB SOURCES
#        *.cpp       # 匹配所有 .cpp 文件
#        model/*.cpp
#        redis/*.cpp
#)
#
## 检查是否找到源文件
#if (NOT SOURCES)
#    message(FATAL_ERROR "No source files found!")
#endif ()
#
## 指定生成可执行文件
##add_executable(ChatServer ${SOURCES})
#
## 排除不需要的文件
##list(FILTER SOURCES EXCLUDE REGEX ".*_test.cpp")
#target_link_libraries(ChatServer muduo_net muduo_base pthread mysqlclient hiredis)