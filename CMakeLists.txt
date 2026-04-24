cmake_minimum_required(VERSION 3.16)
project(NodeFlow VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 已经为你配置好了绝对路径，注意这里必须是正斜杠
set(CMAKE_PREFIX_PATH "D:/Qt/6.8.3/msvc2022_64") 

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

add_executable(NodeFlow
    main.cpp
    NodeItem.h
    NodeItem.cpp
    NodeView.h    # 新加的
    NodeView.cpp  # 新加的
    EdgeItem.h
    EdgeItem.cpp
    NumberNode.h
    NumberNode.cpp
    DisplayNode.h
    DisplayNode.cpp
    AddNode.h
    AddNode.cpp
    NodeFactory.h
    NodeFactory.cpp
)

target_link_libraries(NodeFlow PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)