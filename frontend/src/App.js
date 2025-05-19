import React from 'react';
import 'bootstrap/dist/css/bootstrap.min.css';
import { Tabs, Tab, Modal, Form, Button } from 'react-bootstrap';
import WidgetBoard from './components/WidgetBoard';
import WidgetSettingsPanel from './components/WidgetSettingsPanel';
import Settings from './components/Settings';
import { widgetSchemas } from './data/widgetSchema';

function App() {
  const [key, setKey] = React.useState('widgets');
  const [widgets, setWidgets] = React.useState([]);
  const [showAddModal, setShowAddModal] = React.useState(false);
  const [newWidgetType, setNewWidgetType] = React.useState('slider');
  const [availablePins, setAvailablePins] = React.useState([]);
  const [wsConnected, setWsConnected] = React.useState(false);
  const [sensorSources, setSensorSources] = React.useState([]);

  React.useEffect(() => {
    fetch('/api/pins')
      .then(res => res.json())
      .then(data => {
        if (Array.isArray(data)) {
          setAvailablePins(data.map(pin => ({
            value: pin.pin,
            label: pin.label || `GPIO ${pin.pin}`
          })));
        }
      })
      .catch(err => console.error('Fehler beim Laden der verfügbaren Pins:', err));

    // Nach dem Laden der Pins: gespeicherte Widgets laden
    fetch('/api/widgets')
      .then(res => res.json())
      .then(data => {
        if (Array.isArray(data.widgets)) {
          setWidgets(data.widgets);
        } else {
          console.warn('Unerwartete Antwort von /api/widgets:', data);
        }
      })
      .catch(err => console.error('Fehler beim Laden der gespeicherten Widgets:', err));

    // Sensorquellen laden
    fetch('/api/sensors')
      .then(res => res.json())
      .then(data => {
        if (Array.isArray(data)) {
          setSensorSources(data);
        }
      })
      .catch(err => console.error('Fehler beim Laden der Sensorquellen:', err));
  }, []);

  React.useEffect(() => {
    if (!window.socket) {
      const socket = new WebSocket(`ws://${window.location.hostname}/ws`);
      window.socket = socket;

      socket.onopen = () => {
        console.log("✅ WebSocket verbunden");
        setWsConnected(true);
      };

      socket.onclose = () => {
        console.warn("❌ WebSocket getrennt");
        setWsConnected(false);
      };

      socket.onerror = (err) => {
        console.error("WebSocket-Fehler:", err);
        setWsConnected(false);
      };
    }
  }, []);

  const handleSliderChange = (id, value) => {
    console.log('Slider changed:', id, value);
  };

  const handleButtonClick = (id) => {
    console.log('Button clicked:', id);
  };

  const moveWidgetUp = (index) => {
    if (index > 0) {
      const updated = [...widgets];
      [updated[index - 1], updated[index]] = [updated[index], updated[index - 1]];
      setWidgets(updated);
    }
  };

  const moveWidgetDown = (index) => {
    if (index < widgets.length - 1) {
      const updated = [...widgets];
      [updated[index], updated[index + 1]] = [updated[index + 1], updated[index]];
      setWidgets(updated);
    }
  };

  const handleAddWidget = () => {
    setShowAddModal(true);
  };

  const handleConfirmAddWidget = () => {
    const label = `Neues ${newWidgetType}`;
    const newWidget = {
      label,
      type: newWidgetType,
      options: widgetSchemas[newWidgetType]?.options.map(opt => ({
        ...opt,
        value: opt.default ?? ''
      })) ?? []
    };
    setWidgets([...widgets, newWidget]);
    setShowAddModal(false);
  };

  const handleSaveWidgets = () => {
    fetch('/api/widgets', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ widgets })
    })
      .then(res => {
        if (!res.ok) throw new Error('Fehler beim Speichern der Widgets');
        return res.json();
      })
      .then(() => console.log('Widgets erfolgreich gespeichert.'))
      .catch(err => console.error('Fehler:', err.message));
  };

  return (
    <>
    <div class="container mt-3">
      <div className="text-end text-muted px-3">
        WebSocket:{" "}
        <span style={{ color: wsConnected ? "green" : "red" }}>
          {wsConnected ? "verbunden" : "getrennt"}
        </span>
      </div>
      <Tabs id="main-tabs" activeKey={key} onSelect={(k) => setKey(k)} className="mb-3">
        <Tab eventKey="widgets" title="Widgets">
          <WidgetBoard
            widgets={widgets}
            availablePins={availablePins}
            handleSliderChange={handleSliderChange}
            handleButtonClick={handleButtonClick}
          />
        </Tab>
        <Tab eventKey="widget-settings" title="Widget-Einstellungen">
          <WidgetSettingsPanel
            widgets={widgets}
            setWidgets={setWidgets}
            availablePins={availablePins}
            moveWidgetUp={moveWidgetUp}
            moveWidgetDown={moveWidgetDown}
            onSave={handleSaveWidgets}
            sensorSources={sensorSources}
          />
        </Tab>
        <Tab eventKey="settings" title="Allgemeine Einstellungen">
          <Settings availablePins={availablePins} />
        </Tab>
        <Modal show={showAddModal} onHide={() => setShowAddModal(false)}>
          <Modal.Header closeButton>
            <Modal.Title>Widget hinzufügen</Modal.Title>
          </Modal.Header>
          <Modal.Body>
            <Form.Group>
              <Form.Label>Widget-Typ</Form.Label>
              <Form.Select
                value={newWidgetType}
                onChange={(e) => setNewWidgetType(e.target.value)}
              >
                <option value="video">Video</option>
                <option value="joystick">Joystick</option>
                <option value="slider">Slider</option>
                <option value="button">Button</option>
                <option value="sensor">Sensor</option>
              </Form.Select>
            </Form.Group>
          </Modal.Body>
          <Modal.Footer>
            <Button variant="secondary" onClick={() => setShowAddModal(false)}>
              Abbrechen
            </Button>
            <Button variant="primary" onClick={handleConfirmAddWidget}>
              Hinzufügen
            </Button>
          </Modal.Footer>
        </Modal>
      </Tabs>
    </div>
    </>
  );
}

export default App;
