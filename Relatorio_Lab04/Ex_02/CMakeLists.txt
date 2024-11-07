cmake_minimum_required(VERSION 2.8)
project( Equalize_Calc_Hist )

# Encontrar o OpenCV
find_package( OpenCV REQUIRED )

# Incluir diretórios do OpenCV
include_directories( ${OpenCV_INCLUDE_DIRS} )

# Adicionar o executável
add_executable( Equalize_Calc_Hist Equalize_Calc_Hist.cpp )

# Lincar bibliotecas do OpenCV
target_link_libraries( Equalize_Calc_Hist ${OpenCV_LIBS} )


