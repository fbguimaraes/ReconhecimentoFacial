import face_recognition
import cv2

camera = cv2.VideoCapture(0)
print("Posicione seu rosto na câmera e pressione 's' para capturar.")

while True:
    ret, frame = camera.read()
    cv2.imshow("Captura de Rosto", frame)
    if cv2.waitKey(1) & 0xFF == ord('s'):
        cv2.imwrite("images/usuario.jpg", frame)
        break

camera.release()
cv2.destroyAllWindows()

imagem = face_recognition.load_image_file("images/usuario.jpg")
codificacao = face_recognition.face_encodings(imagem)[0]

import pickle
with open("images/usuario.pkl", "wb") as f:
    pickle.dump(codificacao, f)

print("Rosto cadastrado com sucesso!")

