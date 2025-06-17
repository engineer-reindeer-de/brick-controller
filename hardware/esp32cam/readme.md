Projekt: ESP32-CAM Control Board (5x7cm)

# Bauteile
- ESP32-CAM Header (2x8 Pins)
- OLED SSD1306 (4-Pin I2C Header)
- 4x SG90 Servo-Header (3-Pin Dupont: GND, VCC, PWM)
- I2C Expansion Header (4-Pin Dupont: VCC, GND, SDA, SCL)
- Power Input (2-Pin Dupont: GND, 5V)
- Power Switch zwischen Power Input und 5V-Schiene
- Serial Port (5-Pin Dupont: GND, 5V, 3.3V, TX, RX)
- Button GPIO0 → GND
- Reset-Button (EN/RST → GND)
- Optional: LDO-Regler 5V → 3.3V

# Gehäuse/Abmessungen
- Boardgröße: 50mm x 70mm
- Befestigungslöcher in den vier Ecken (M2)

# Layout-Vorschlag
- Oben Mitte: ESP32-CAM
- Links: OLED + I2C Header
- Rechts oben: Power Input + Power Switch
- Rechts unten: Serial-Port
- Unten: 4x Servo Header
- Mitte: Buttons für GPIO0 & Reset

# Hinweise
- 5V-Leitung: Dicke Leiterbahn für Servos
- 3.3V nur für ESP32 und OLED
- Pull-Ups für I2C optional (4.7k)
- GPIO0-Button nahe Serial-Port platzieren
- Leiterbahnen nach Strombedarf auslegen (Servos ~500mA pro Stück)
