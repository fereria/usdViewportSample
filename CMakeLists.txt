cmake_minimum_required( VERSION 3.6 )

project(SampleProj CXX)

find_package(imgui CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

set(USD_INSTALL_ROOT C:/USD)
set(USD_LIBRARY_DIRECTORY ${USD_INSTALL_ROOT}/lib)
set(USD_INCLUDE_DIRECTORY ${USD_INSTALL_ROOT}/include)
set(BOOST_INCLUDE_DIRECTORY ${USD_INSTALL_ROOT}/include/boost-1_70)

find_library(USD_SDF usd_sdf HINTS ${USD_LIBRARY_DIRECTORY})
find_library(USD_TF usd_tf HINTS ${USD_LIBRARY_DIRECTORY})
find_library(USD_USD usd_usd HINTS ${USD_LIBRARY_DIRECTORY})
find_library(USD_GEOM usd_usdGeom HINTS ${USD_LIBRARY_DIRECTORY})
find_library(TBB_LIB tbb HINTS ${USD_LIBRARY_DIRECTORY})
find_library(IMG_LIB usd_usdImagingGL HINTS ${USD_LIBRARY_DIRECTORY})
find_library(GF_LIB usd_gf HINTS ${USD_LIBRARY_DIRECTORY})
find_library(GLF_LIB usd_glf HINTS ${USD_LIBRARY_DIRECTORY})

set(PYTHON_ROOT_DIR C:/Users/remir/AppData/Local/Programs/Python/Python39)

add_executable(${CMAKE_PROJECT_NAME} main.cpp MainWindow.cpp SimpleCamera.cpp)

include_directories(${CMAKE_PROJECT_NAME}
                    PRIVATE
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${USD_INCLUDE_DIRECTORY}
                    ${BOOST_INCLUDE_DIRECTORY}
                    ${PYTHON_ROOT_DIR}/include
                    )

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE opengl32.lib
                                            glfw
                                            imgui::imgui
                                            ${USD_USD}
                                            ${TBB_LIB}
                                            ${USD_SDF}
                                            ${USD_TF}
                                            ${USD_GEOM}
                                            ${IMG_LIB}
                                            ${GF_LIB}
                                            ${GLF_LIB}
                                            ${USD_LIBRARY_DIRECTORY}/boost_python39-vc142-mt-x64-1_70.lib
                                            ${PYTHON_ROOT_DIR}/libs/python39.lib)

