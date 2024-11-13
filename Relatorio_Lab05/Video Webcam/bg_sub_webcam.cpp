#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>

using namespace cv;
using namespace std;

int main()
{
    // Criação do subtrator de fundo
    Ptr<BackgroundSubtractor> pBackSub = createBackgroundSubtractorMOG2();

    // Abertura da webcam
    VideoCapture capture(0);
    if (!capture.isOpened()) {
        cerr << "Não foi possível abrir a webcam." << endl;
        return -1;
    }

    // Criação do writer para salvar o vídeo de saída
    VideoWriter writer("resultado_webcam.avi", VideoWriter::fourcc('M','J','P','G'), 30, Size(capture.get(CAP_PROP_FRAME_WIDTH), capture.get(CAP_PROP_FRAME_HEIGHT)));

    Mat frame, fgMask;

    while (capture.read(frame)) {
        // Aplicar a subtração de fundo
        pBackSub->apply(frame, fgMask);

        // Criação de imagem com objetos em movimento
        Mat moving_objects;
        frame.copyTo(moving_objects, fgMask);

        // Exibir a imagem original e a máscara de movimento
        imshow("Webcam - Original", frame);
        imshow("Webcam - FG Mask", fgMask);
        
        // Salvar no vídeo de saída
        writer.write(moving_objects);

        if (waitKey(30) == 'q')
            break;
    }

    capture.release();
    writer.release();
    destroyAllWindows();

    return 0;
}

