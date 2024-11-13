#include <iostream>
#include <sstream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
    // Criação do subtrator de fundo
    Ptr<BackgroundSubtractor> pBackSub = createBackgroundSubtractorMOG2();

    // Carregar os vídeos de movimento rápido e lento
    VideoCapture capture_slow("objeto_lento.mp4");
    VideoCapture capture_fast("objeto_rapido.mp4");

    if (!capture_slow.isOpened() || !capture_fast.isOpened()) {
        cerr << "Não foi possível abrir um dos vídeos." << endl;
        return -1;
    }

    // Criação dos writers para gravar os vídeos de saída
    VideoWriter writer_slow("resultado_lento.avi", VideoWriter::fourcc('M','J','P','G'), 30, Size(capture_slow.get(CAP_PROP_FRAME_WIDTH), capture_slow.get(CAP_PROP_FRAME_HEIGHT)));
    VideoWriter writer_fast("resultado_rapido.avi", VideoWriter::fourcc('M','J','P','G'), 30, Size(capture_fast.get(CAP_PROP_FRAME_WIDTH), capture_fast.get(CAP_PROP_FRAME_HEIGHT)));

    Mat frame, fgMask;

    // Processa cada frame do vídeo com movimento lento
    while (capture_slow.read(frame)) {
        pBackSub->apply(frame, fgMask);
        Mat moving_objects;
        frame.copyTo(moving_objects, fgMask);

        imshow("Movimento Lento - Frame", frame);
        imshow("Movimento Lento - FG Mask", fgMask);
        writer_slow.write(moving_objects);

        if (waitKey(30) == 'q')
            break;
    }

    // Processa cada frame do vídeo com movimento rápido
    while (capture_fast.read(frame)) {
        pBackSub->apply(frame, fgMask);
        Mat moving_objects;
        frame.copyTo(moving_objects, fgMask);

        imshow("Movimento Rápido - Frame", frame);
        imshow("Movimento Rápido - FG Mask", fgMask);
        writer_fast.write(moving_objects);

        if (waitKey(30) == 'q')
            break;
    }

    capture_slow.release();
    capture_fast.release();
    writer_slow.release();
    writer_fast.release();
    destroyAllWindows();
    
    return 0;
}
