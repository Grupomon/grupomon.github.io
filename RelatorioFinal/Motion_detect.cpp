#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>
#include <ctime>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

// Configurações do alarme e da aproximação
const int alert_trigger_time_15s = 15;     // Tempo em segundos para alerta de movimento
const int alert_trigger_time_1m = 60;      // Tempo em segundos para alerta de possível intruso
const int alert_trigger_time_2m = 120;     // Tempo em segundos para alerta de perigo intruso
const int no_movement_reset_time = 10;     // Tempo em segundos sem movimento para resetar o temporizador
const double close_area_threshold = 5000.0; // Limite de área para considerar uma aproximação perigosa (ajustável)
const double far_distance_threshold = 3000.0; // Limite de área para considerar um objeto como longe

bool first_detection_logged = false;
bool photo_taken = false;
bool video_recording_started = false;

// Função para obter o horário atual no formato "Dia_Mês_Ano_Hora_Minuto_Segundo"
std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm* local_time = std::localtime(&now_time);

    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%d_%m_%Y_%H_%M_%S", local_time);
    return std::string(buffer);
}

// Função para salvar uma foto
void savePhoto(const cv::Mat& frame) {
    std::string directory = "Captura_foto";
    std::filesystem::create_directories(directory); // Garante que a pasta exista

    std::string filename = directory + "/" + getCurrentTimestamp() + ".jpg";
    cv::imwrite(filename, frame);
    std::cout << "Foto salva em: " << filename << std::endl;
}

// Função para iniciar gravação de vídeo
cv::VideoWriter startVideoRecording(const cv::Size& frame_size, int fps) {
    std::string directory = "Captura_video";
    std::filesystem::create_directories(directory); // Garante que a pasta exista

    std::string filename = directory + "/" + getCurrentTimestamp() + ".avi";
    return cv::VideoWriter(filename, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, frame_size);
}

// Função para salvar o log em um arquivo .txt
void saveLogToFile(const std::vector<std::string>& full_log, const std::string& filename) {
    std::ofstream log_file(filename);
    if (log_file.is_open()) {
        for (const auto& message : full_log) {
            log_file << message << "\n";
        }
        log_file.close();
        std::cout << "Log salvo em: " << filename << std::endl;
    } else {
        std::cerr << "Erro ao salvar o log em: " << filename << std::endl;
    }
}

// Função para exibir texto com fundo escuro translúcido
void drawTextWithBackground(cv::Mat& frame, const std::vector<std::string>& lines, int x, int y, double font_scale, int font_thickness, cv::Scalar text_color, cv::Scalar bg_color) {
    int line_spacing = 25; // Espaçamento entre as linhas
    int baseline = 0;

    for (size_t i = 0; i < lines.size(); ++i) {
        int y_offset = y + i * line_spacing;
        cv::Size text_size = cv::getTextSize(lines[i], cv::FONT_HERSHEY_SIMPLEX, font_scale, font_thickness, &baseline);

        // Desenhar fundo escuro translúcido
        cv::rectangle(frame, cv::Point(x, y_offset - text_size.height - 5), cv::Point(x + text_size.width + 10, y_offset + baseline), bg_color, cv::FILLED);

        // Desenhar texto
        cv::putText(frame, lines[i], cv::Point(x + 5, y_offset), cv::FONT_HERSHEY_SIMPLEX, font_scale, text_color, font_thickness, cv::LINE_AA);
    }
}

int main() {
    cv::VideoCapture cap(0); // Conecte à câmera USB

    if (!cap.isOpened()) {
        std::cerr << "Erro ao abrir a câmera!" << std::endl;
        return -1;
    }

    cv::Mat frame, fgMask;
    cv::Ptr<cv::BackgroundSubtractor> pBackSub = cv::createBackgroundSubtractorMOG2(); // Subtração de fundo
    cv::VideoWriter video_writer;

    auto last_detected = std::chrono::steady_clock::now();
    auto last_movement_time = std::chrono::steady_clock::now();
    bool person_detected = false;
    bool alert_15s_logged = false;
    bool alert_1m_logged = false;
    bool alert_2m_logged = false;
    bool movement_occurred = false;
    bool is_close = false;

    int fps = cap.get(cv::CAP_PROP_FPS);
    cv::Size frame_size = cv::Size((int)cap.get(cv::CAP_PROP_FRAME_WIDTH), (int)cap.get(cv::CAP_PROP_FRAME_HEIGHT));

    std::vector<std::string> message_log; // Armazena mensagens para exibição acumulada
    std::vector<std::string> full_log;    // Armazena todas as mensagens para salvar no arquivo

    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        // Subtração de fundo para detectar movimento
        pBackSub->apply(frame, fgMask);

        // Encontrar contornos
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(fgMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        bool movement_detected = false;
        for (const auto& contour : contours) {
            double area = cv::contourArea(contour);

            if (area > 500) { // Tamanho mínimo para detectar uma pessoa
                movement_detected = true;
                movement_occurred = true; // Marca que um movimento ocorreu

                if (!first_detection_logged) {
                    std::string msg = "Movimento detectado: primeiro movimento no campo de visao.";
                    message_log.push_back(msg);
                    full_log.push_back(msg);
                    first_detection_logged = true;
                }

                // Retângulo ao redor do objeto
                cv::Rect bounding_rect = cv::boundingRect(contour);
                cv::rectangle(frame, bounding_rect, cv::Scalar(0, 255, 0), 2);

                // Define a proximidade com base na área
                if (area > close_area_threshold) {
                    is_close = true;
                } else if (area < far_distance_threshold) {
                    is_close = false;
                }

                if (!person_detected) {
                    last_detected = std::chrono::steady_clock::now();
                    person_detected = true;
                }
            }
        }

        // Atualiza o tempo do último movimento detectado
        if (movement_detected) {
            last_movement_time = std::chrono::steady_clock::now();
        }

        // Calcula o tempo desde o último movimento detectado
        auto now = std::chrono::steady_clock::now();
        double time_since_last_movement = std::chrono::duration_cast<std::chrono::seconds>(now - last_movement_time).count();

        // Se não houver movimento por mais de 10 segundos, reseta o temporizador e alertas
        if (time_since_last_movement > no_movement_reset_time) {
            if (movement_occurred) {
                std::string msg = "Nao foi nada, apenas alguem ou algo passou.";
                message_log.push_back(msg);
                full_log.push_back(msg);
            }
            person_detected = false;
            alert_15s_logged = false;
            alert_1m_logged = false;
            alert_2m_logged = false;
            first_detection_logged = false;
            last_detected = now; // Reinicia o temporizador
            movement_occurred = false; // Reset para detectar novo movimento
            photo_taken = false; // Permite tirar nova foto em próxima detecção
            video_recording_started = false; // Permite iniciar nova gravação
        }

        // Temporizador de presença prolongada
        double seconds = std::chrono::duration_cast<std::chrono::seconds>(now - last_detected).count();

        // Exibir mensagens de alerta baseadas no tempo de permanência
        if (person_detected && seconds >= no_movement_reset_time) { // Ignorar movimentos rápidos menores que 10s
            std::string distance_message = is_close ? " de perto" : " de longe";

            if (seconds >= alert_trigger_time_15s && !alert_15s_logged) {
                std::string msg = "Alerta de movimento: presenca detectada por mais de 15 segundos" + distance_message + ".";
                message_log.push_back(msg);
                full_log.push_back(msg);
                alert_15s_logged = true;

                // Captura de foto se ainda não tirou
                if (!photo_taken) {
                    savePhoto(frame);
                    photo_taken = true;
                }
            }
            if (seconds >= alert_trigger_time_1m && !alert_1m_logged) {
                std::string msg = "Alerta de possivel intruso: presenca detectada por mais de 1 minuto" + distance_message + ".";
                message_log.push_back(msg);
                full_log.push_back(msg);
                alert_1m_logged = true;

                // Inicia gravação de vídeo se ainda não começou
                if (!video_recording_started) {
                    video_writer = startVideoRecording(frame_size, fps);
                    video_recording_started = true;
                }
            }
            if (seconds >= alert_trigger_time_2m && !alert_2m_logged) {
                std::string msg = "Perigo! Intruso detectado por mais de 2 minutos" + distance_message + ".";
                message_log.push_back(msg);
                full_log.push_back(msg);
                alert_2m_logged = true;
            }
        }

        // Gravação de vídeo se iniciada
        if (video_recording_started && seconds <= alert_trigger_time_1m + 60) {
            video_writer.write(frame);
        } else if (video_recording_started && seconds > alert_trigger_time_1m + 60) {
            video_writer.release(); // Para a gravação após 1 minuto
            video_recording_started = false;
            std::string msg = "Gravacao de video do intruso finalizada apos 1 minuto.";
            message_log.push_back(msg);
            full_log.push_back(msg);
        }

        // Exibição das mensagens acumuladas no canto superior esquerdo com fundo translúcido
        drawTextWithBackground(frame, message_log, 10, 30, 0.5, 1, cv::Scalar(0, 0, 255), cv::Scalar(0, 0, 0, 100));

        // Exibição do frame e da máscara de movimento
        cv::imshow("Monitoramento", frame);
        cv::imshow("Movimento Detectado", fgMask);

        // Fecha o programa ao pressionar Q ou ESC
        int key = cv::waitKey(30);
        if (key == 27 || key == 'q' || key == 'Q') break;
    }

    cap.release();
    if (video_writer.isOpened()) video_writer.release();

    // Salva o log completo no final da execução
    saveLogToFile(full_log, "log_intrucoes.txt");

    cv::destroyAllWindows();
    return 0;
}
