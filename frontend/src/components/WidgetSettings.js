import React from 'react';
import { Button } from 'react-bootstrap';
import { widgetSchemas } from '../data/widgetSchema';
import { Form } from 'react-bootstrap';

function WidgetSettings({ widget, index, widgets, setWidgets, availablePins, moveUp, moveDown }) {
  if (typeof widget.type !== 'string') {
    console.warn('Ungültiger widget.type:', widget.type);
    return null;
  }
  

  const updateOption = (optIdx, newValue) => {
    const updated = [...widgets];
    updated[index].options[optIdx].value = newValue;
    setWidgets(updated);
    console.log('Aktuelle config:', JSON.stringify({ widgets: updated }, null, 2));
  };

  const schema = widgetSchemas?.[widget.type];
  if (!schema) {
    console.warn(`Kein Schema gefunden für Widget-Typ: ${widget.type}`);
    return null;
  }

  const removeWidget = (indexToRemove) => {
    const updated = widgets.filter((_, i) => i !== indexToRemove);
    setWidgets(updated);
  };
  
  return (
    <div className="mb-4 p-3 border rounded bg-light">
      <div className="d-flex justify-content-between mb-2">
      <h5>{widget.label} ({widget.type})</h5>
      <div className="btn-group btn-group-sm">
          <Button
          size="sm"
          variant="outline-secondary"
          onClick={() => moveUp(index)}
          disabled={index === 0}
          title="Nach oben"
        >
          ↑
        </Button>
        <Button
          size="sm"
          variant="outline-secondary"
          onClick={() => moveDown(index)}
          disabled={index === widgets.length - 1}
          title="Nach unten"
        >
          ↓
        </Button>

        <Button
          variant="outline-danger"
          size="sm"
          onClick={() => removeWidget(index)}
          title="Widget entfernen"
        >
          ✕
        </Button>
        </div>
      </div>
      {Array.isArray(widget.options) && widget.options.map((opt, idx) => {
        const type = opt.type || 'string';

        return (
          <Form.Group key={idx} className="mb-2">
            <Form.Label>{opt.label}</Form.Label>
            {type === 'boolean' ? (
              <Form.Check
                type="checkbox"
                checked={opt.value === true}
                onChange={(e) => updateOption(idx, e.target.checked)}
                label={opt.label}
              />
            ) : type === 'integer' || type === 'int' ? (
              opt.name === 'pin' || opt.name === 'xPin' || opt.name === 'yPin' ? (
                <Form.Select
                  value={opt.value}
                  onChange={(e) => updateOption(idx, parseInt(e.target.value, 10))}
                >
                  {availablePins.map((pin) => (
                    <option key={pin.value} value={pin.value}>{pin.label}</option>
                  ))}
                </Form.Select>
              ) : (
                <Form.Control
                  type="number"
                  value={opt.value}
                  onChange={(e) => updateOption(idx, parseInt(e.target.value, 10))}
                />
              )
            ) : (
              <Form.Control
                type="text"
                value={opt.value}
                onChange={(e) => updateOption(idx, e.target.value)}
              />
            )}
          </Form.Group>
        );
      })}
    </div>
  );
}

export default WidgetSettings;