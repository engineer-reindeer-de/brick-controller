# Verwende ein Basis-Image mit Arduino-CLI
FROM jpconstantineau/arduino-cli

# Arbeitsverzeichnis im Container
WORKDIR /app

# Kopiere den gesamten Projektinhalt in das Arbeitsverzeichnis
COPY . .

# Installiere ESP32 Board-Paket
RUN arduino-cli core update-index && \
    arduino-cli core install esp32:esp32

# Kompiliere den Sketch
RUN arduino-cli compile --fqbn esp32:esp32:esp32cam main.cpp

# Hinweis: Das Flashen auf das Board muss auf dem Host erfolgen