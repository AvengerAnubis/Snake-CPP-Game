project(client)

set (SOURCE_FILES
    main.cpp shader.cpp
)

set(TARGET_NAME snake)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(TARGET_NAME snake_debug)
endif()

add_executable(${TARGET_NAME} "")

target_sources(${TARGET_NAME} PRIVATE
    ${SOURCE_FILES}
)

set_target_properties(${TARGET_NAME} PROPERTIES
RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/
)


target_include_directories(${TARGET_NAME} PRIVATE 
    ${PROJECT_BINARY_DIR}/include/
)


add_dependencies(${TARGET_NAME} glfw glew_s)
target_link_libraries(${TARGET_NAME} glfw glew_s)

file(   COPY ${CMAKE_SOURCE_DIR}/include 
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)
file(   COPY ${CMAKE_CURRENT_SOURCE_DIR}/level.lvl
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)
file(   COPY ${CMAKE_CURRENT_SOURCE_DIR}/shader.h
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)