import cv2
import face_recognition
import serial
import time
import threading

# Configuração da comunicação serial (mude a porta se necessário)
ser = serial.Serial('COM6', 115200, timeout=1)  # No Linux: '/dev/ttyUSB0'

# Carrega a imagem do rosto autorizado
imagem_autorizada = face_recognition.load_image_file("images/usuario.jpg")
codificacao_autorizada = face_recognition.face_encodings(imagem_autorizada)[0]

# Inicializa a câmera
video_capture = cv2.VideoCapture(0)
video_capture.set(3, 640)  # Largura do vídeo
video_capture.set(4, 480)  # Altura do vídeo

# Variáveis globais para controle do reconhecimento
rosto_reconhecido = False
frame_para_processar = None
lock = threading.Lock()

def enviar_sinal(acesso_liberado):
    """Envia '1' se o rosto for reconhecido, senão envia '0'."""
    ser.write(b'1' if acesso_liberado else b'0')
    time.sleep(0.1)  # Pequeno atraso para evitar excesso de envio

def reconhecimento_thread():
    """Thread separada para processar o reconhecimento facial sem travar o vídeo."""
    global rosto_reconhecido, frame_para_processar

    while True:
        if frame_para_processar is not None:
            with lock:
                rgb_frame = cv2.cvtColor(frame_para_processar, cv2.COLOR_BGR2RGB)
                face_locations = face_recognition.face_locations(rgb_frame)
                face_encodings = face_recognition.face_encodings(rgb_frame, face_locations)

                rosto_reconhecido = any(
                    face_recognition.compare_faces([codificacao_autorizada], face_encoding)[0]
                    for face_encoding in face_encodings
                )

                frame_para_processar = None  # Libera a variável para o próximo frame
                enviar_sinal(rosto_reconhecido)

        time.sleep(0.1)  # Reduz o uso da CPU

# Inicia a thread para reconhecimento facial
threading.Thread(target=reconhecimento_thread, daemon=True).start()

frame_count = 0

while True:
    ret, frame = video_capture.read()
    if not ret:
        continue  # Se falhar, tenta pegar outro frame

    # Processa um frame a cada 5 capturas para otimizar o desempenho
    frame_count += 1
    if frame_count % 5 == 0:
        with lock:
            frame_para_processar = frame.copy()

    # Desenha um retângulo no rosto identificado
    for (top, right, bottom, left) in face_recognition.face_locations(frame):
        cor = (0, 255, 0) if rosto_reconhecido else (0, 0, 255)  # Verde se reconhecido, vermelho se não
        cv2.rectangle(frame, (left, top), (right, bottom), cor, 2)

    cv2.imshow('Reconhecimento Facial', frame)

    # Pressione 'q' para sair do programa
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Finaliza tudo antes de sair
video_capture.release()
cv2.destroyAllWindows()
ser.close()
