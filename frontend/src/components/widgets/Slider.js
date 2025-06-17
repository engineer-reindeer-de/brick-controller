import React, { useState, useEffect } from 'react';

// Declare lastSend outside the component to persist across renders and calls
let lastSend = 0;

function Slider({ label, pin, onChange }) {
  const [value, setValue] = useState(127);

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
              value: newValue
            });
            window.socket?.send(msg);
          }}
        />
      </div>
    </div>
  );
}

export default Slider;