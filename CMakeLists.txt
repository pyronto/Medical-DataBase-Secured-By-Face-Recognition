cmake_minimum_required(VERSION 3.5)
project(FaceRecognition)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

include_directories(/opt/homebrew/Cellar/hiredis/1.2.0/include)
link_directories(/opt/homebrew/Cellar/hiredis/1.2.0/lib)

add_executable(face_detection face_detection.cpp)
target_link_libraries(face_detection ${OpenCV_LIBS})

add_executable(face_recognition face_recognition.cpp)
target_link_libraries(face_recognition ${OpenCV_LIBS})

add_executable(store_person_data store_person_data.cpp)
target_link_libraries(store_person_data hiredis)

add_executable(retrieve_person_data retrieve_person_data.cpp)
target_link_libraries(retrieve_person_data ${OpenCV_LIBS} hiredis)
