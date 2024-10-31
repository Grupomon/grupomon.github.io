cmake_minimum_required(VERSION 2.8)
project( calcHist_Demo )

# Encontrar o OpenCV
find_package( OpenCV REQUIRED )

# Incluir diretórios do OpenCV
include_directories( ${OpenCV_INCLUDE_DIRS} )

# Adicionar o executável
add_executable( calcHist_Demo calcHist_Demo.cpp )

# Lincar bibliotecas do OpenCV
target_link_libraries( calcHist_Demo ${OpenCV_LIBS} )


