#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main() {
    // Inicializa a captura da webcam
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cerr << "Erro ao abrir a webcam!" << endl;
        return -1;
    }

    Mat frame, filtered_frame;
    char key;
    int kernelSize = 3; // Tamanho do kernel padrão
    int filterType = 0; // 0: Média, 1: Gaussiano, 2: Mediana, 3: Bilateral

    while (true) {
        // Captura um novo quadro da webcam
        cap >> frame;
        if (frame.empty()) {
            cerr << "Erro ao capturar imagem da webcam!" << endl;
            break;
        }

        // Aplica o filtro selecionado
        switch (filterType) {
            case 0: // Filtro de média
                blur(frame, filtered_frame, Size(kernelSize, kernelSize));
                break;
            case 1: // Filtro gaussiano
                GaussianBlur(frame, filtered_frame, Size(kernelSize, kernelSize), 0);
                break;
            case 2: // Filtro mediano
                medianBlur(frame, filtered_frame, kernelSize);
                break;
            case 3: // Filtro bilateral
                bilateralFilter(frame, filtered_frame, kernelSize, kernelSize * 2, kernelSize / 2);
                break;
            default:
                filtered_frame = frame; // Sem filtro
                break;
        }

        // Exibe a imagem filtrada
        imshow("Filtragem da Webcam", filtered_frame);

        // Espera pela tecla do usuário
        key = waitKey(30);
        if (key == 'a') {
            filterType = 0; // Média
        } else if (key == 'g') {
            filterType = 1; // Gaussiano
        } else if (key == 'm') {
            filterType = 2; // Mediana
        } else if (key == 'b') {
            filterType = 3; // Bilateral
        } else if (key == '3') {
            kernelSize = 3;
        } else if (key == '5') {
            kernelSize = 5;
        } else if (key == '7') {
            kernelSize = 7;
        } else if (key == '9') {
            kernelSize = 9;
        } else if (key == 'B') {
            kernelSize = 11;
        } else if (key == 'q') {
            break; // Sai do loop se a tecla 'q' for pressionada
        }
    }

    // Libera a captura e fecha as janelas
    cap.release();
    destroyAllWindows();

    return 0;
}

