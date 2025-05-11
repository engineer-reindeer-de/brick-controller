# Makefile

# Name der HTML-Datei
HTML_FILE = index.html

# Name der generierten Header-Datei
HEADER_FILE = src/index_html.h

# Ziel: Umwandlung der HTML-Datei in ein C-Array
build:
	xxd -i $(HTML_FILE) > $(HEADER_FILE)
	@echo "Header-Datei $(HEADER_FILE) wurde erstellt."

# Ziel: Öffnen der HTML-Datei im Browser
.PHONY: open
open: $(HTML_FILE)
	open $(HTML_FILE)
	@echo "HTML-Datei $(HTML_FILE) im Browser geöffnet."

# Ziel: Alles ausführen
.PHONY: all
all: $(HEADER_FILE)

# Ziel: Aufräumen
.PHONY: clean
clean:
	rm -f $(HEADER_FILE)
	@echo "Bereinigt: $(HEADER_FILE) wurde entfernt."

# Kompilieren des Sketches mit PlatformIO
.PHONY: compile
compile:
	platformio run
	@echo "Kompilierung abgeschlossen."
# Flashen des ESP32-CAM mit PlatformIO
.PHONY: flash
flash: build compile
	platformio run --target upload
	@echo "ESP32-CAM wurde geflasht."
# Ziel: Alles ausführen
.PHONY: all
all: $(HEADER_FILE)

# Serial Monitor öffnen mit PlatformIO
.PHONY: monitor
monitor:
	platformio device monitor

# Verfügbare serielle Ports auflisten
.PHONY: list_ports
list_ports:
	@echo "Verfügbare serielle Ports:"
	@ls /dev/tty.* /dev/cu.* 2>/dev/null || echo "Keine seriellen Ports gefunden."

.PHONY: start-server
start-server:
	docker build -t esp32-cockpit-ui ./frontend
	docker run -p 3000:80 esp32-cockpit-ui

build-ui:
	docker run -it --rm -v "$(shell pwd)/frontend:/app" -w /app node:20-alpine npm run build
	rm -rf data/*
	cp -r frontend/build/* data/
	rm -rf data/static/js/*.map data/static/js/*.LICENSE.txt data/static/css/*.map

uploadfs: build-ui
	pio run -e esp32cam -t erase 
	pio run -e esp32cam -t upload
	pio run -e esp32cam -t uploadfs