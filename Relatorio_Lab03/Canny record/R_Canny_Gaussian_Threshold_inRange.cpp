#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>

using namespace cv;

Mat frame, frame_gray;
Mat dst, detected_edges;

int lowThreshold = 0;
const int max_lowThreshold = 100;
const int ratio = 3;
const int kernel_size = 3;
const char* window_name = "Canny Edge Map";

VideoWriter videoWriter; // Para gravar o vídeo
bool isRecording = false; // Flag para controle de gravação

// Função para aplicar o detector Canny
static void CannyThreshold(int, void*)
{
    // Converter a imagem para escala de cinza
    cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
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
    
    // Criar trackbar para ajustar o limiar
    createTrackbar("Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold);

    while (true)
    {
        cap >> frame; // Captura um frame da webcam
        if (frame.empty())
            break;

        // Chamar a função Canny para processar a imagem
        CannyThreshold(0, 0);

        // Exibir a imagem com bordas detectadas
        imshow("Original", frame);
        imshow(window_name, dst);

        // Salvar a imagem ao pressionar 's'
        char key = (char)waitKey(30);
        if (key == 's') // Salvar imagem
        {
            imwrite("canny_image.png", dst); // Salvar a imagem processada pelo Canny
            std::cout << "Image saved as 'canny_image.png'" << std::endl;
        }
        else if (key == 'k') // Iniciar gravação de vídeo
        {
            if (!isRecording)
            {
                videoWriter.open("output_canny.avi", VideoWriter::fourcc('M','J','P','G'), 10, frame.size());
                if (!videoWriter.isOpened())
                {
                    std::cerr << "Error: Could not open the video writer!" << std::endl;
                    return -1;
                }
                isRecording = true;
                std::cout << "Recording started..." << std::endl;
            }
        }
        else if (key == 'h') // Parar gravação de vídeo
        {
            if (isRecording)
            {
                videoWriter.release();
                isRecording = false;
                std::cout << "Recording stopped." << std::endl;
            }
        }

        // Gravar o frame se estiver gravando
        if (isRecording)
        {
            videoWriter.write(dst); // Gravar a imagem processada pelo Canny
        }
    }

    cap.release(); // Liberar a captura de vídeo
    videoWriter.release(); // Liberar o gravador de vídeo se estiver aberto
    destroyAllWindows(); // Fechar todas as janelas

    return 0;
}
