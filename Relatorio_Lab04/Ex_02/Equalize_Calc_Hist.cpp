#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;

void plotHistogram(const Mat& image, Mat& histImage) {
    int histSize = 256;
    float range[] = { 0, 256 };
    const float* histRange[] = { range };
    bool uniform = true, accumulate = false;

    // Calcular histogramas para os canais B, G e R
    Mat b_hist, g_hist, r_hist;
    vector<Mat> bgr_planes;
    split(image, bgr_planes);
    calcHist(&bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, histRange, uniform, accumulate);
    calcHist(&bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, histRange, uniform, accumulate);
    calcHist(&bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, histRange, uniform, accumulate);

    // Normalizar os histogramas
    normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
    normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
    normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());

    // Desenhar os histogramas
    int hist_w = 512;
    int bin_w = cvRound((double)hist_w / histSize);

    for (int i = 1; i < histSize; i++) {
        line(histImage, Point(bin_w * (i - 1), histImage.rows - cvRound(b_hist.at<float>(i - 1))),
             Point(bin_w * i, histImage.rows - cvRound(b_hist.at<float>(i))),
             Scalar(255, 0, 0), 2, 8, 0); // Canal B

        line(histImage, Point(bin_w * (i - 1), histImage.rows - cvRound(g_hist.at<float>(i - 1))),
             Point(bin_w * i, histImage.rows - cvRound(g_hist.at<float>(i))),
             Scalar(0, 255, 0), 2, 8, 0); // Canal G

        line(histImage, Point(bin_w * (i - 1), histImage.rows - cvRound(r_hist.at<float>(i - 1))),
             Point(bin_w * i, histImage.rows - cvRound(r_hist.at<float>(i))),
             Scalar(0, 0, 255), 2, 8, 0); // Canal R
    }
}

int main() {
    VideoCapture cap(0); // Captura da câmera padrão
    if (!cap.isOpened()) {
        cout << "Erro ao abrir a câmera!" << endl;
        return -1;
    }

    while (true) {
        Mat frame;
        cap >> frame; // Captura o quadro da webcam

        if (frame.empty()) {
            cout << "Quadro vazio, encerrando!" << endl;
            break;
        }

        // Converter para escala de cinza
        Mat gray;
        cvtColor(frame, gray, COLOR_BGR2GRAY);

        // Aplicar equalização de histograma
        Mat equalized;
        equalizeHist(gray, equalized);

        // Gerar histogramas
        Mat histImage(400, 512, CV_8UC3, Scalar(0, 0, 0));
        plotHistogram(frame, histImage);

        // Exibir imagens
        imshow("Imagem Original", frame);
        imshow("Imagem em Escala de Cinza", gray);
        imshow("Imagem Equalizada", equalized);
        imshow("Histograma", histImage);

        // Espera por uma tecla
        char c = (char)waitKey(30);
        if (c == 'q') {
            break; // Encerra se 'q' for pressionado
        } else if (c == 's') {
            // Salva todas as imagens e histogramas
            imwrite("imagem_original.png", frame);
            imwrite("imagem_gray.png", gray);
            imwrite("imagem_equalizada.png", equalized);
            imwrite("histograma_imagem.png", histImage);
            cout << "Imagens e histogramas salvos!" << endl;
        }
    }

    // Libera a captura e fecha todas as janelas
    cap.release();
    destroyAllWindows();

    return 0;
}
