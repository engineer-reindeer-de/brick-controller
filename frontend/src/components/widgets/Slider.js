import React, { useState, useEffect } from 'react';

// Declare lastSend outside the component to persist across renders and calls
let lastSend = 0;

function Slider({ label, pin, snapback, onChange, offset, drift }) {
  const [value, setValue] = useState(127);

  useEffect(() => {
    if (snapback && value !== 127) {
      const timeout = setTimeout(() => {
        setValue(127);
        if (onChange) onChange(pin, 127);
        const msg = JSON.stringify({
          target: { type: "pwm", chip: "esp32", pin },
          value: 127,
          offset: offset,
          drift: drift
        });
        console.log(msg);
        window.socket?.send(msg);
      }, 300); // delay in ms

      return () => clearTimeout(timeout);
    }
  }, [value, snapback]);

  return (
    <div className="card mb-3">
      <div className="card-header">{label || `Slider (Pin ${pin})`}</div>
      <div className="card-body">
        <input
          type="range"
          min="0"
          max="255"
          value={value}
          className="form-range"
          onChange={(e) => {
            const now = Date.now();
            if (now - lastSend < 100) return;
            lastSend = now;

            const newValue = Number(e.target.value);
            setValue(newValue);
            if (onChange) onChange(pin, newValue);
            const msg = JSON.stringify({
              target: { type: "pwm", chip: "esp32", pin },
              value: newValue,
              offset: offset,
              drift: drift
            });
            console.log(msg);
            window.socket?.send(msg);
          }}
        />
      </div>
    </div>
  );
}

export default Slider;