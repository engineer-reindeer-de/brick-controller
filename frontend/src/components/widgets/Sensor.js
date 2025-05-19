import React from 'react';

function Sensor({ label, value }) {
  return (
    <div className="card mb-3">
      <div className="card-header">{label}</div>
      <div className="card-body text-center">
        <span className="display-6">{value}</span>
      </div>
    </div>
  );
}

export default Sensor;