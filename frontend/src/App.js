import React, { useEffect, useState } from 'react';
import 'bootstrap/dist/css/bootstrap.min.css';

import VideoStream from './components/VideoStream';
import Slider from './components/Slider';
import Button from './components/Button';
import Sensor from './components/Sensor';
import Navbar from './components/Navbar';
import Joystick from './components/Joystick';
import WidgetSettings from './components/WidgetSettings';

import { mapOptions } from './utils/mapOptions'; // optional, falls verwendet
import { Modal, Button as BsButton, Form } from 'react-bootstrap';
import { widgetSchemas } from './data/widgetSchema';

function App() {
  const [widgets, setWidgets] = useState([]);
  const [newWidgetType, setNewWidgetType] = useState('');
  const [newWidgetOptions, setNewWidgetOptions] = useState([]);
  const [showAddModal, setShowAddModal] = useState(false);
  const [availablePins] = useState([
    { value: 2, label: 'GPIO 2 – LED' },
    { value: 4, label: 'GPIO 4 – I2C SDA' },
    { value: 5, label: 'GPIO 5 – I2C SCL' },
    { value: 12, label: 'GPIO 12 – Servo X' },
    { value: 13, label: 'GPIO 13 – Servo Y' },
    { value: 14, label: 'GPIO 14 – PWM 1' },
    { value: 15, label: 'GPIO 15 – PWM 2' }
  ]);
  const [wsConnected, setWsConnected] = useState(false);

  useEffect(() => {
    const socket = new WebSocket('ws://' + window.location.hostname + '/ws');
    window.socket = socket;

    socket.onopen = () => setWsConnected(true);
    socket.onclose = () => setWsConnected(false);
    socket.onerror = () => setWsConnected(false);

    return () => socket.close();
  }, []);

  useEffect(() => {
    fetch('/api/widgets')
      .then((res) => res.json())
      .then((data) => {
        if (Array.isArray(data.widgets)) {
          setWidgets(data.widgets);
          console.log('Widgets vom Server geladen:', JSON.stringify(data, null, 2));
        } else {
          console.warn('Keine gültigen Widgets im Response:', data);
        }
      })
      .catch((err) => {
        console.error('Fehler beim Abrufen der Widgets:', err);
      });
  }, []);

  const handleSliderChange = (pin, value) => {
    console.log(`Slider: Pin ${pin}, Wert ${value}`);
    // z. B. per WebSocket senden
  };

  const handleButtonClick = (action) => {
    console.log(`Button-Action: ${action}`);
    // z. B. per WebSocket senden
  };

  // Move up/down widget functions
  const moveWidgetUp = (index) => {
    if (index === 0) return;
    const updated = [...widgets];
    [updated[index - 1], updated[index]] = [updated[index], updated[index - 1]];
    setWidgets(updated);
  };

  const moveWidgetDown = (index) => {
    if (index === widgets.length - 1) return;
    const updated = [...widgets];
    [updated[index], updated[index + 1]] = [updated[index + 1], updated[index]];
    setWidgets(updated);
  };


  return (
    <>

      <div className="container py-4">
        <div className="card mb-4">
          <div className="card-body">
            <h1 className="card-title">ESP32 Cockpit</h1>
            <p>Status: {wsConnected ? "🟢 WebSocket verbunden" : "🔴 keine Verbindung"}</p>
            <p className="card-text">Steuere dein Gerät über dynamische UI-Komponenten.</p>
          </div>
        </div>

        <div id="dashboard" className="row g-4">
          {widgets.map((widget, index) => {
            const props = {
              key: index,
              label: widget.label,
              ...mapOptions(widget.options || [])
            };

            const colClass = "col-12 col-md-6 col-lg-4";

            switch (widget.type) {
              case 'video':
                return <div className={colClass}><VideoStream {...props} /></div>;
              case 'joystick':
                return <div className={colClass}><Joystick {...props} /></div>;
              case 'slider':
                return <div className={colClass}><Slider {...props} onChange={handleSliderChange} /></div>;
              case 'button':
                return <div className={colClass}><Button {...props} onClick={handleButtonClick} /></div>;
              case 'sensor':
                return <div className={colClass}><Sensor {...props} /></div>;
              default:
                return null;
            }
          })}
        </div>

        {/* Einstellungen-Bereich */}
        <div className="card mt-5" id="settings">
          <div className="card-header d-flex justify-content-between align-items-center">
            <span>Widget-Konfiguration</span>
            <BsButton
              variant="success"
              size="sm"
              onClick={() => setShowAddModal(true)}
            >
              ＋ hinzufügen
            </BsButton>
          <BsButton
            variant="primary"
            size="sm"
            onClick={() => {
              fetch('/api/widgets', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ widgets })
              })
              .then(res => res.ok ? alert("Gespeichert ✅") : alert("Fehler beim Speichern ❌"))
              .catch(err => {
                console.error("Fehler beim Speichern:", err);
                alert("Netzwerkfehler beim Speichern ❌");
              });
            }}
          >
            speichern
          </BsButton>
          </div>
          
          <div className="card-body">
            {widgets.map((widget, index) => (
              <div className="d-flex align-items-start justify-content-between mb-3" key={index}>
                <div className="flex-grow-1 me-3">
                  <WidgetSettings
                    widget={widget}
                    index={index}
                    widgets={widgets}
                    setWidgets={setWidgets}
                    availablePins={availablePins}
                    moveUp={moveWidgetUp}
                    moveDown={moveWidgetDown}
                  />
                </div>
              </div>
            ))}
          </div>
        </div>
      </div>
      {/* Modal für neues Widget */}
      <Modal show={showAddModal} onHide={() => setShowAddModal(false)}>
        <Modal.Header closeButton>
          <Modal.Title>Neues Widget hinzufügen</Modal.Title>
        </Modal.Header>
        <Modal.Body>
          <Form>
            <Form.Group className="mb-3">
              <Form.Label>Typ auswählen</Form.Label>
              <Form.Select
                value={newWidgetType}
                onChange={(e) => setNewWidgetType(e.target.value)}
              >
                <option value="">-- auswählen --</option>
                <option value="video">Video</option>
                <option value="joystick">Joystick</option>
                <option value="slider">Slider</option>
                <option value="button">Button</option>
                <option value="sensor">Sensor</option>
              </Form.Select>
            </Form.Group>
          </Form>
        </Modal.Body>
        <Modal.Footer>
          <BsButton variant="secondary" onClick={() => setShowAddModal(false)}>Abbrechen</BsButton>
          <BsButton
            variant="primary"
            onClick={() => {
              const schema = widgetSchemas[newWidgetType];
              const newWidget = {
                type: newWidgetType,
                label: schema?.label || (newWidgetType[0].toUpperCase() + newWidgetType.slice(1)),
                options: schema?.options.map(({ name, label, type, default: value }) => ({
                  name, label, type, value
                })) || []
              };
              const updatedWidgets = [...widgets, newWidget];
              setWidgets(updatedWidgets);
              console.log('Aktuelle config:', JSON.stringify({ widgets: updatedWidgets }, null, 2));
              setNewWidgetType('');
              setNewWidgetOptions([]);
              setShowAddModal(false);
            }}
            disabled={!newWidgetType}
          >
            Hinzufügen
          </BsButton>
        </Modal.Footer>
      </Modal>
    </>
  );
}

export default App;