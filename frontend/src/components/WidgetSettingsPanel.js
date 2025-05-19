import React from 'react';
import WidgetSettings from './WidgetSettings';
import { Button as BsButton } from 'react-bootstrap';
import { Modal, Form } from 'react-bootstrap';
import { widgetSchemas } from '../data/widgetSchema';

function WidgetSettingsPanel({
  widgets,
  setWidgets,
  availablePins,
  moveWidgetUp,
  moveWidgetDown,
  onSave,
  sensorSources = []
}) {
  const [showAddModal, setShowAddModal] = React.useState(false);
  const [newWidgetType, setNewWidgetType] = React.useState('slider');

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

  return (
    <div className="card mt-5" id="settings">
      <div className="card-header d-flex justify-content-between align-items-center">
        <span>Widget-Konfiguration</span>
        <div className="btn-group">
          <BsButton variant="success" size="sm" onClick={() => {
            console.log('Hinzufügen geklickt');
            setShowAddModal(true);
          }}>
            ＋ hinzufügen
          </BsButton>
          <BsButton variant="primary" size="sm" onClick={onSave}>speichern</BsButton>
        </div>
      </div>

      <div className="card-body">
        {(Array.isArray(widgets) ? widgets : []).map((widget, index) => {
            if (
              !widget ||
              typeof widget !== 'object' ||
              !Array.isArray(widget.options) ||
              typeof widget.type !== 'string'
            ) {
              console.warn('Ungültiges Widget übersprungen:', widget);
              return null;
            }

            return (
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
                    sensorSources={sensorSources}
                  />
                </div>
              </div>
            );
          })}
      </div>
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
          <BsButton variant="secondary" onClick={() => setShowAddModal(false)}>
            Abbrechen
          </BsButton>
          <BsButton variant="primary" onClick={handleConfirmAddWidget}>
            Hinzufügen
          </BsButton>
        </Modal.Footer>
      </Modal>
    </div>
  );
}

export default WidgetSettingsPanel;