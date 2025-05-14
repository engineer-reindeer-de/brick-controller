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

# Serial Monitor öffnen mit PlatformIO
.PHONY: monitor
monitor:
	platformio device monitor

# Verfügbare serielle Ports auflisten
.PHONY: list_ports
list_ports:
	@echo "Verfügbare serielle Ports:"
	@ls /dev/tty.* /dev/cu.* 2>/dev/null || echo "Keine seriellen Ports gefunden."

install-server:
	docker run -it --rm -v "$(shell pwd)/frontend:/app" -w /app node:20-alpine npm install

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