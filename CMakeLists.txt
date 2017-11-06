cmake_minimum_required(VERSION 2.6)
  
project(OPENGLEXAMPLES)

option(BUILD_OGL43 "Bild OpenGL 4.3 examples" OFF)
mark_as_advanced(BUILD_OGL43)

find_package(OpenGL REQUIRED)

add_subdirectory(glfw)
add_subdirectory(glxw)

include_directories(glfw/include)
include_directories(glm)
include_directories(${CMAKE_BINARY_DIR}/glxw/include)
 
set(CMAKE_CXX_FLAGS "-O2 -Wall -Wextra")
set(CMAKE_CXX_FLAGS "-O2 -Wall")
SET(LIBRARIES glfw glxw ${GLFW_LIBRARIES} ${GLXW_LIBRARY} ${OPENGL_LIBRARY} ${CMAKE_DL_LIBS})

link_directories (${OPENGLEXAMPLES_BINARY_DIR}/bin)
 
set(projects
basic
00skeleton
01shader_vbo1
01shader_vbo2
02indexed_vbo
03texture
04perspective
05fbo_fxaa
06instancing1
06instancing2_buffer_texture
06instancing3_uniform_buffer
07geometry_shader_blending
08map_buffer
09transform_feedback
10queries_conditional_render
11tesselation
12shader_image_load_store
13compute_shader_nbody
)

foreach(project ${projects})

project(${project})
add_executable(${project} ${project}.cpp)
target_link_libraries(${project} ${LIBRARIES})

endforeach(project)
