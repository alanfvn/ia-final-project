import uuid
import cv2
import numpy as np
from PIL import Image
from folders import get_images, get_processed_file

images = get_images()
face_classifier = cv2.CascadeClassifier(f'{cv2.data.haarcascades}haarcascade_frontalface_default.xml')
img_w, img_h = 224, 224
label_ids = {}

for id, label in enumerate(images.keys()):
    imgs = images[label]
    label_ids[id] = label

    for img in imgs:
        # load the image
        imgtest = cv2.imread(img, cv2.IMREAD_COLOR)
        img_array = np.array(imgtest, "uint8")

        # detect the faces
        faces = face_classifier.detectMultiScale(imgtest, scaleFactor=1.1, minNeighbors=5)

        if len(faces) != 1:
            print(f'Skipped photo: {label}: {img}')
            continue

        for (x, y, w, h) in faces:
            # draw the detected face
            face_detect = cv2.rectangle(imgtest, (x,y), (x+w, y+h), (255, 0, 255), 2)
            size = (img_w, img_h)
            # detected face region
            roi = img_array[y: y+h, x: x+w]
            # resize the detected face to target size
            resized_image = cv2.resize(roi, size)
            img_array = np.array(resized_image, "uint8")
            im = Image.fromarray(img_array)
            res = get_processed_file(label, f"{uuid.uuid4()}.png")
            im.save(res)