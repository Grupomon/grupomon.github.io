#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main() {
    // Inicializa a captura da webcam
    VideoCapture cap(0); // 0 é o ID da webcam padrão
    if (!cap.isOpened()) {
        cerr << "Erro ao abrir a webcam!" << endl;
        return -1;
    }

    Mat frame, filtered_frame;
    char key;

    while (true) {
        // Captura um novo quadro da webcam
        cap >> frame;

        // Verifica se o quadro foi capturado
        if (frame.empty()) {
            cerr << "Erro ao capturar imagem da webcam!" << endl;
            break;
        }

        // Aplica o filtro gaussiano
        GaussianBlur(frame, filtered_frame, Size(11, 11), 0);

        // Exibe a imagem filtrada
        imshow("Filtro Gaussiano - Webcam", filtered_frame);

        // Espera pela tecla 's' para salvar a imagem ou 'q' para sair
        key = waitKey(30);
        if (key == 's') {
            imwrite("imagem_filtrada.jpg", filtered_frame);
            cout << "Imagem salva como 'imagem_filtrada.jpg'" << endl;
        }
        if (key == 'q') {
            break; // Sai do loop se a tecla 'q' for pressionada
        }
    }

    // Libera a captura e fecha as janelas
    cap.release();
    destroyAllWindows();

    return 0;
}

