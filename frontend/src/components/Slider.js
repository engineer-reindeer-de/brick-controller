import React from 'react';

function Slider({ label, pin, onChange }) {
  return (
    <div className="card mb-3">
      <div className="card-header">{label || `Slider (Pin ${pin})`}</div>
      <div className="card-body">
        <input
          type="range"
          min="0"
          max="255"
          className="form-range"
          onChange={(e) => onChange && onChange(pin, e.target.value)}
        />
      </div>
    </div>
  );
}

export default Slider;