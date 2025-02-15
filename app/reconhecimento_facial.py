import cv2
import face_recognition
import serial
import time
import threading

# Configuração da comunicação serial
ser = serial.Serial('COM6', 115200, timeout=1)  # No Linux use '/dev/ttyUSB0'

# Carregar a imagem do rosto autorizado
imagem = face_recognition.load_image_file("images/usuario.jpg")
codificacao = face_recognition.face_encodings(imagem)[0]

# Inicializa a captura de vídeo
video_capture = cv2.VideoCapture(0)
video_capture.set(3, 640)  # Reduz largura
video_capture.set(4, 480)  # Reduz altura

# Variáveis globais
rosto_reconhecido = False
frame_to_process = None
lock = threading.Lock()

def enviar_sinal(presenca):
    ser.write(b'1' if presenca else b'0')
    time.sleep(0.1)  # Delay para evitar sobrecarga

# Thread para reconhecimento facial
def reconhecimento_thread():
    global rosto_reconhecido, frame_to_process

    while True:
        if frame_to_process is not None:
            with lock:
                rgb_frame = cv2.cvtColor(frame_to_process, cv2.COLOR_BGR2RGB)
                face_locations = face_recognition.face_locations(rgb_frame)
                face_encodings = face_recognition.face_encodings(rgb_frame, face_locations)

                encontrado = any(
                    face_recognition.compare_faces([codificacao], face_encoding)[0]
                    for face_encoding in face_encodings
                )

                rosto_reconhecido = encontrado
                frame_to_process = None
                enviar_sinal(rosto_reconhecido)

        time.sleep(0.1)  # Evita uso excessivo da CPU

# Inicia a thread de reconhecimento facial
threading.Thread(target=reconhecimento_thread, daemon=True).start()

frame_count = 0

while True:
    ret, frame = video_capture.read()
    if not ret:
        continue

    # Processa a cada 5 frames
    frame_count += 1
    if frame_count % 5 == 0:
        with lock:
            frame_to_process = frame.copy()

    # Exibir vídeo com retângulo no rosto
    for (top, right, bottom, left) in face_recognition.face_locations(frame):
        cor = (0, 255, 0) if rosto_reconhecido else (0, 0, 255)
        cv2.rectangle(frame, (left, top), (right, bottom), cor, 2)

    cv2.imshow('Video', frame)

    # Pressione 'q' para sair
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Finalizar
video_capture.release()
cv2.destroyAllWindows()
ser.close()
