import React from 'react';

function Navigation() {
  return (
    <nav className="navbar navbar-expand-lg navbar-dark bg-dark mb-4">
      <div className="container-fluid">
        <a className="navbar-brand" href="#">ESP32 Cockpit</a>
        <button className="navbar-toggler" type="button" data-bs-toggle="collapse" data-bs-target="#navbarNav"
          aria-controls="navbarNav" aria-expanded="false" aria-label="Toggle navigation">
          <span className="navbar-toggler-icon"></span>
        </button>
        <div className="collapse navbar-collapse" id="navbarNav">
          <ul className="navbar-nav">
            <li className="nav-item">
              <a className="nav-link active" href="#dashboard">Dashboard</a>
            </li>
            <li className="nav-item">
              <a className="nav-link" href="#settings">Einstellungen</a>
            </li>
            <li className="nav-item">
              <a className="nav-link" href="#">Über</a>
            </li>
          </ul>
        </div>
      </div>
    </nav>
  );
}

export default Navigation;