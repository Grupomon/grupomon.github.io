cmake_minimum_required(VERSION 2.8)
project( bg_sub_webcam )

# Encontrar o OpenCV
find_package( OpenCV REQUIRED )

# Incluir diretórios do OpenCV
include_directories( ${OpenCV_INCLUDE_DIRS} )

# Adicionar o executável
add_executable( bg_sub_webcam bg_sub_webcam.cpp )

# Lincar bibliotecas do OpenCV
target_link_libraries( bg_sub_webcam ${OpenCV_LIBS} )
