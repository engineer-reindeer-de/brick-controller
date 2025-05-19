import React from 'react';

function Button({ label, action, onClick }) {
  return (
    <div className="card mb-3">
      <div className="card-body text-center">
        <button
          className="btn btn-primary"
          onClick={() => onClick && onClick(action)}
        >
          {label}
        </button>
      </div>
    </div>
  );
}

export default Button;