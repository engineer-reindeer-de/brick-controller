import React from 'react';
import VideoStream from './widgets/VideoStream';
import Slider from './widgets/Slider';
import Button from './widgets/Button';
import Sensor from './widgets/Sensor';
import Joystick from './widgets/Joystick';

function WidgetBoard({ widgets, availablePins, handleSliderChange, handleButtonClick }) {
  // Check that widgets is an array
  if (!Array.isArray(widgets)) {
    console.warn('widgets ist kein Array:', widgets);
    widgets = [];
  }

  const COMPONENTS = {
    video: VideoStream,
    joystick: (props) => <Joystick {...props} availablePins={availablePins} />,
    slider: (props) => <Slider {...props} onChange={handleSliderChange} />,
    button: (props) => <Button {...props} onClick={handleButtonClick} />,
    sensor: Sensor,
  };

  return (
    <div id="dashboard" className="row g-4">
      {widgets.map((widget, index) => {
        const props = {
          key: index,
          label: widget.label,
          ...Object.fromEntries((widget.options || []).map(opt => [opt.name, opt.value]))
        };

        const colClass = "col-12 col-md-6 col-lg-4";

        const WidgetComponent = COMPONENTS[widget.type];
        if (!WidgetComponent) {
          console.warn(`Unbekannter Widget-Typ: ${widget.type}`);
          return null;
        }
        return (
          <div className={colClass} key={index}>
            {typeof WidgetComponent === 'function' && WidgetComponent.prototype && WidgetComponent.prototype.isReactComponent
              ? <WidgetComponent {...props} />
              : WidgetComponent(props)}
          </div>
        );
      })}
    </div>
  );
}

export default WidgetBoard;