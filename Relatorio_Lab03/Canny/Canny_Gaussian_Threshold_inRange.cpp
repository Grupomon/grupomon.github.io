#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>

using namespace cv;

Mat frame, frame_gray;
Mat dst, detected_edges;
Mat filtered_image;

int lowThreshold = 0;
const int max_lowThreshold = 100;
const int ratio = 3;
const int kernel_size = 3;
const char* window_name = "Canny Edge Map";
const char* window_filtered_name = "Filtered Image";

// Função para aplicar o detector Canny
static void CannyThreshold(int, void*)
{
    blur(frame_gray, detected_edges, Size(3, 3));
    Canny(detected_edges, detected_edges, lowThreshold, lowThreshold * ratio, kernel_size);
    dst = Scalar::all(0);
    frame.copyTo(dst, detected_edges);
    imshow(window_name, dst);
}

int main(int argc, char** argv)
{
    VideoCapture cap(0); // Captura de vídeo da webcam
    if (!cap.isOpened())
    {
        std::cerr << "Error: Cannot open the webcam!" << std::endl;
        return -1;
    }

    namedWindow(window_name, WINDOW_AUTOSIZE);
    namedWindow(window_filtered_name, WINDOW_AUTOSIZE);

    // Criar trackbar para ajustar o limiar
    createTrackbar("Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold);

    while (true)
    {
        cap >> frame; // Captura um frame da webcam
        if (frame.empty())
            break;

        // Converter a imagem para escala de cinza
        cvtColor(frame, frame_gray, COLOR_BGR2GRAY);

        // Exemplo de filtragem: aplicar um desfoque gaussiano
        GaussianBlur(frame, filtered_image, Size(15, 15), 0);

        // Chamar a função Canny pela primeira vez
        CannyThreshold(0, 0);

        // Exibir a imagem filtrada
        imshow(window_filtered_name, filtered_image);

        // Mostrar a imagem original (opcional)
        imshow("Original", frame);

        // Esperar por uma tecla
        char key = (char)waitKey(30);
        if (key == 'q' || key == 27) // Sair se 'q' ou 'Esc' for pressionado
            break;
    }

    cap.release(); // Liberar a captura de vídeo
    destroyAllWindows(); // Fechar todas as janelas

    return 0;
}
