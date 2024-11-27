#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>

using namespace std;
using namespace cv;

/** Função para detectar e exibir rostos e olhos */
void detectAndDisplay(Mat& frame, CascadeClassifier& face_cascade, CascadeClassifier& eyes_cascade) {
    Mat frame_gray;
    cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
    equalizeHist(frame_gray, frame_gray);

    // Detectar rostos
    vector<Rect> faces;
    face_cascade.detectMultiScale(frame_gray, faces);

    for (size_t i = 0; i < faces.size(); i++) {
        Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
        ellipse(frame, center, Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, Scalar(255, 0, 255), 4);

        Mat faceROI = frame_gray(faces[i]);

        // Detectar olhos dentro do rosto
        vector<Rect> eyes;
        eyes_cascade.detectMultiScale(faceROI, eyes);

        for (size_t j = 0; j < eyes.size(); j++) {
            Point eye_center(faces[i].x + eyes[j].x + eyes[j].width / 2,
                             faces[i].y + eyes[j].y + eyes[j].height / 2);
            int radius = cvRound((eyes[j].width + eyes[j].height) * 0.25);
            circle(frame, eye_center, radius, Scalar(255, 0, 0), 4);
        }
    }

    // Exibir a imagem processada
    imshow("Detecção de Rostos e Olhos", frame);
}

int main() {
    // Carregar os classificadores Haarcascade
    CascadeClassifier face_cascade;
    CascadeClassifier eyes_cascade;

    string face_cascade_name = "haarcascade_frontalface_alt.xml"; // Classificador para rostos humanos
    string eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml"; // Classificador para olhos

    if (!face_cascade.load(face_cascade_name)) {
        cout << "--(!) Erro ao carregar o modelo XML de rosto: " << face_cascade_name << endl;
        return -1;
    }
    if (!eyes_cascade.load(eyes_cascade_name)) {
        cout << "--(!) Erro ao carregar o modelo XML de olhos: " << eyes_cascade_name << endl;
        return -1;
    }

    // Abrir a webcam
    VideoCapture capture(0); // '0' indica a câmera padrão do sistema
    if (!capture.isOpened()) {
        cout << "--(!) Erro ao abrir a webcam" << endl;
        return -1;
    }

    Mat frame;

    while (true) {
        capture >> frame; // Capturar o próximo frame da webcam
        if (frame.empty()) {
            cout << "--(!) Quadro vazio capturado -- Saindo!" << endl;
            break;
        }

        // Processar o quadro capturado
        detectAndDisplay(frame, face_cascade, eyes_cascade);

        // Salvar a imagem ao pressionar 'S'
        char key = (char)waitKey(10); // Espera por uma tecla pressionada
        if (key == 's' || key == 'S') {
            string output_filename = "output_detected.jpg"; // Nome do arquivo de saída
            imwrite(output_filename, frame);               // Salvar a imagem processada
            cout << "Imagem salva como: " << output_filename << endl;
        }

        // Sair ao pressionar 'ESC'
        if (key == 27) { // Código ASCII para ESC
            break;
        }
    }

    return 0;
}

