cmake_minimum_required(VERSION 3.0)

project(Snake)


add_subdirectory(deps/glfw glfw)
add_subdirectory(deps/glew/build/cmake glew_s)
add_subdirectory(src client)

