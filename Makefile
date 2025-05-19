.PHONY: build
build:
	cd firmware && pio run
	@echo "Kompilierung abgeschlossen."

.PHONY: upload
upload:
	cd firmware && pio run --target upload
	@echo "ESP32-CAM wurde geflasht."

.PHONY: monitor
monitor:
	cd firmware && pio device monitor

.PHONY: list_ports
list_ports:
	@echo "Verfügbare serielle Ports:"
	@ls /dev/tty.* /dev/cu.* 2>/dev/null || echo "Keine seriellen Ports gefunden."

.PHONY: install-server
install-server:
	docker run -it --rm -v "$(shell pwd)/frontend:/app" -w /app node:20-alpine npm install

.PHONY: start-server
start-server:
	docker build -t esp32-cockpit-ui ./frontend
	docker run -p 3000:80 esp32-cockpit-ui

.PHONY: build-ui
build-ui:
	docker run -it --rm -v "$(shell pwd)/frontend:/app" -w /app node:20-alpine npm run build
	rm -rf firmware/data/*
	cp -r frontend/build/* firmware/data/
	rm -rf firmware/data/static/js/*.map firmware/data/static/js/*.LICENSE.txt firmware/data/static/css/*.map

.PHONY: uploadfs
uploadfs: build-ui
	cd firmware && pio run -e esp32cam -t erase && pio run -e esp32cam -t uploadfs