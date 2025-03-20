file(GLOB SOURCES
        *.cpp       # 匹配所有 .cpp 文件
)

# 指定生成可执行文件
add_executable(ChatClient ${SOURCES})
# 指定可执行文件链接时需要依赖的库文件
target_link_libraries(ChatClient pthread)