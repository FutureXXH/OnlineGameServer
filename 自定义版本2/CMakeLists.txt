project(GameServer)

cmake_minimum_required(VERSION 2.8)

include_directories(./include)
link_directories(${cmake_CURRENT_SOURCE_DIR}/lua/)
link_directories(${cmake_CURRENT_SOURCE_DIR}/lib/)


aux_source_directory(./src DIR_SOURCE)
aux_source_directory(./src/Modules DIR_MODULE)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread -g")

add_executable(GameServer ${DIR_SOURCE} ${DIR_MODULE})



target_link_libraries(GameServer liblua.a)
target_link_libraries(GameServer dl)