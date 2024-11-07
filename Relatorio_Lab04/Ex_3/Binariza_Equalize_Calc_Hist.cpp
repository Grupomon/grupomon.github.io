#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int threshold_value = 128; // Valor inicial para o limiar
int threshold_type = 0;    // Tipo de limiar (binário)
int const max_value = 255;
int const max_type = 4;
int const max_binary_value = 255;

Mat src, src_gray, dst, equalized;
const char* window_name = "Binarização e Equalização";

// Função de callback para a trackbar de tipo de limiar
void Threshold_Demo(int, void*) {
    // Aplica a binarização após a equalização de histograma
    threshold(equalized, dst, threshold_value, max_binary_value, threshold_type);
    if (!dst.empty()) {
        imshow(window_name, dst);
    }
}

// Função para plotar o histograma
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

    namedWindow(window_name, WINDOW_AUTOSIZE); // Criação de janela para exibição da binarização

    // Trackbars com callbacks
    createTrackbar("Tipo de Limiar", window_name, &threshold_type, max_type, Threshold_Demo);
    createTrackbar("Valor do Limiar", window_name, &threshold_value, max_value, Threshold_Demo);

    while (true) {
        Mat frame;
        cap >> frame; // Captura o quadro da webcam

        if (frame.empty()) {
            cout << "Quadro vazio, encerrando!" << endl;
            break;
        }

        // Converter para escala de cinza
        cvtColor(frame, src_gray, COLOR_BGR2GRAY);

        // Aplicar equalização de histograma
        equalizeHist(src_gray, equalized);

        // Gerar histogramas
        Mat histImage(400, 512, CV_8UC3, Scalar(0, 0, 0));
        plotHistogram(frame, histImage);

        // Exibir imagens
        imshow("Imagem Original", frame);
        imshow("Imagem em Escala de Cinza", src_gray);
        imshow("Imagem Equalizada", equalized);

        // Exibe o histograma
        imshow("Histograma", histImage);
        
        // Realizar a binarização da imagem após equalização
        Threshold_Demo(0, 0);

        // Espera por uma tecla
        char c = (char)waitKey(30);
        if (c == 'q') {
            break; // Encerra se 'q' for pressionado
        } else if (c == 's') {
            // Salva todas as imagens e histogramas
            imwrite("imagem_original.png", frame);
            imwrite("imagem_gray.png", src_gray);
            imwrite("imagem_equalizada.png", equalized);
            imwrite("histograma_imagem.png", histImage);
            imwrite("imagem_binarizada.png", dst);
            cout << "Imagens e histogramas salvos!" << endl;
        }
    }

    // Libera a captura e fecha todas as janelas
    cap.release();
    destroyAllWindows();

    return 0;
}

