cmake_minimum_required(VERSION 3.16)
project(vtk_model_builder)

set(CMAKE_CXX_STANDARD 17)

find_package(VTK REQUIRED)
find_package(DICOM REQUIRED)
find_package(Eigen3 REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)
link_libraries(${JSONCPP_LIBRARIES})

include(${VTK_USE_FILE})
include(${DICOM_USE_FILE})

set(SOURCES
  src/main.cpp
  src/config_reader.cpp
  src/dcm_reader.cpp
  src/model_builder.cpp
  src/scene_provider.cpp
)

add_executable(vtk_model_builder ${SOURCES})

target_link_libraries(vtk_model_builder 
    ${VTK_LIBRARIES}
    vtkDICOM
    gdcmMSFF
    Eigen3::Eigen
)
