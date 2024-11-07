#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <sstream>
#include <filesystem>  // Para verificar e criar diretórios (C++17+)

using namespace cv;
using namespace std;
namespace fs = std::filesystem; // Usado para manipulação de arquivos e diretórios

// Função para plotar o histograma de uma imagem
void plotHistogram(const Mat& image, Mat& histImage) {
    int histSize = 256;
    float range[] = { 0, 256 };
    const float* histRange[] = { range };
    bool uniform = true, accumulate = false;

    // Calcular os histogramas para os canais B, G e R
    vector<Mat> bgr_planes;
    split(image, bgr_planes);
    
    // Calcular histogramas
    Mat b_hist, g_hist, r_hist;
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

// Função principal
int main() {
    // Abrir a câmera padrão (índice 0)
    VideoCapture cap(0); 
    if (!cap.isOpened()) {
        cout << "Erro ao abrir a câmera!" << endl;
        return -1;
    }

    // Criar diretório de saída, se não existir
    string outputDir = "capturas";
    if (!fs::exists(outputDir)) {
        fs::create_directory(outputDir);
        cout << "Diretório 'capturas' criado para salvar as imagens." << endl;
    }

    int frame_count = 0; // Contador de quadros para nomear os arquivos

    while (true) {
        Mat frame;
        cap >> frame; // Captura o quadro da webcam

        if (frame.empty()) {
            cout << "Quadro vazio, encerrando!" << endl;
            break;
        }

        // Dividir a imagem em 3 canais: BGR
        vector<Mat> bgr_planes;
        split(frame, bgr_planes);

        // Equalização nos canais separadamente
        Mat b_equalized, g_equalized, r_equalized;
        equalizeHist(bgr_planes[0], b_equalized);  // Equaliza o canal azul
        equalizeHist(bgr_planes[1], g_equalized);  // Equaliza o canal verde
        equalizeHist(bgr_planes[2], r_equalized);  // Equaliza o canal vermelho

        // Combina os canais equalizados
        Mat dst;
        merge(vector<Mat>{b_equalized, g_equalized, r_equalized}, dst);

        // Exibir a imagem original e a imagem equalizada
        imshow("Imagem Original", frame);
        imshow("Imagem Equalizada", dst);

        // Gerar histogramas para a imagem original
        Mat histImageBefore(400, 512, CV_8UC3, Scalar(0, 0, 0));
        plotHistogram(frame, histImageBefore);

        // Gerar histogramas para a imagem equalizada
        Mat histImageAfter(400, 512, CV_8UC3, Scalar(0, 0, 0));
        plotHistogram(dst, histImageAfter);

        // Exibir os histogramas
        imshow("Histograma Original", histImageBefore);
        imshow("Histograma Equalizado", histImageAfter);

        // Espera por uma tecla
        char c = (char)waitKey(30);
        if (c == 'q') {
            break; // Encerra se 'q' for pressionado
        }
        else if (c == 's') {
            // Salva todas as imagens e histogramas
            stringstream ss;
            ss << frame_count;
            string filename = ss.str();
            
            // Caminho completo para salvar as imagens no diretório "capturas"
            string imgOriginalPath = outputDir + "/imagem_original_" + filename + ".png";
            string imgEqualizedPath = outputDir + "/imagem_equalizada_" + filename + ".png";
            string histOriginalPath = outputDir + "/histograma_original_" + filename + ".png";
            string histEqualizedPath = outputDir + "/histograma_equalizado_" + filename + ".png";

            // Salva as imagens e histogramas
            bool savedOriginal = imwrite(imgOriginalPath, frame); 
            bool savedEqualized = imwrite(imgEqualizedPath, dst);
            bool savedHistOriginal = imwrite(histOriginalPath, histImageBefore);
            bool savedHistEqualized = imwrite(histEqualizedPath, histImageAfter);

            // Verifica se as imagens foram salvas corretamente
            if (savedOriginal && savedEqualized && savedHistOriginal && savedHistEqualized) {
                cout << "Imagens e histogramas salvos com sucesso!" << endl;
            } else {
                cout << "Erro ao salvar as imagens!" << endl;
            }

            // Incrementa o contador para salvar arquivos com nomes diferentes
            frame_count++;
        }
    }

    // Libera a captura e fecha todas as janelas
    cap.release();
    destroyAllWindows();

    return 0;
}

