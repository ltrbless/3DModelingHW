# 设置 CMake 的最低版本要求
cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

# 设定项目
project(3DModeling VERSION 0.1)
message(STATUS "[Project] ${PROJECT_NAME}")

# 修改默认的安装路径前缀
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR} CACHE PATH "install prefix" FORCE)
endif()

# 确定 C++ 版本
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 默认为realse版本
SET(CMAKE_BUILD_TYPE "Release")

#if(MSVC)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /permissive-")
#endif()

# 将生成的 exe, dll, lib 等放置在 bin, lib 目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/lib")

#添加所有源代码
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src sources) 
#添加依赖Imgui框架源代码
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/extern/imgui imgui_dir) 
#添加 glad.c 代码
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/extern/src other_dir) 


message(STATUS "[Current Cmake Dir : ] ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "[Src file list : ] ${sources} ")
#message(STATUS "[Imgui Dir is : ] ${CMAKE_CURRENT_SOURCE_DIR}/extern/include/imgui") 
#message(STATUS "[ImgUi Src file list : ] ${imgui_dir} ")

#生成可执行文件
add_executable(${PROJECT_NAME} ${sources} ${imgui_dir} ${other_dir})

#指定头文件路径
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/extern/eigen)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/extern/glm-0.9.9.8)
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/extern/include")
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/extern)

#指定链接库的目录
target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/extern/lib)

#链接库
target_link_libraries(${PROJECT_NAME} PRIVATE glfw3.lib OpenGL32.Lib)

