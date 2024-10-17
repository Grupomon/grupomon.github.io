/**
 * file Smoothing.cpp
 * brief Sample code for simple filters
 * author OpenCV team
 */

#include <iostream>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

using namespace std;
using namespace cv;

/// Global Variables
char window_name[] = "Smoothing Demo";

Mat src; 
Mat dst;

/**
 * function main
 */
int main(int argc, char** argv) {
    namedWindow(window_name, WINDOW_AUTOSIZE);

    /// Load the source image
    const char* filename = argc >= 2 ? argv[1] : "imagem.jpg"; // Alterado para imagem.jpg

    src = imread(samples::findFile(filename), IMREAD_COLOR);
    if (src.empty()) {
        printf("Error opening image\n");
        printf("Usage:\n %s [image_name-- default imagem.jpg] \n", argv[0]);
        return EXIT_FAILURE;
    }

    // Aplicar filtros e salvar apenas uma imagem para cada tipo
    // Filtro de média
    blur(src, dst, Size(7, 7));
    imwrite("imagem_media.jpg", dst);

    // Filtro gaussiano
    GaussianBlur(src, dst, Size(7, 7), 0, 0);
    imwrite("imagem_gaussiana.jpg", dst);

    // Filtro mediano
    medianBlur(src, dst, 7);
    imwrite("imagem_mediana.jpg", dst);

    // Filtro bilateral
    bilateralFilter(src, dst, 7, 14, 3.5);
    imwrite("imagem_bilateral.jpg", dst);

    // Exibir a imagem original e a filtrada
    imshow("Original Image", src);
    imshow("Filtered Image", dst);
    waitKey(0); // Espera até que uma tecla seja pressionada

    return 0;
}
