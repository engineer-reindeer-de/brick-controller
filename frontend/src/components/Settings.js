import React, { useState, useEffect } from 'react';

function Settings({ availablePins }) {
  const [ssid, setSsid] = useState('');
  const [password, setPassword] = useState('');
  const [i2cSda, setI2cSda] = useState('');
  const [i2cScl, setI2cScl] = useState('');

  useEffect(() => {
    fetch('/api/i2c')
      .then(res => res.json())
      .then(data => {
        if (data?.sda !== undefined) setI2cSda(data.sda);
        if (data?.scl !== undefined) setI2cScl(data.scl);
      })
      .catch(err => console.error('Fehler beim Laden der I2C-Konfiguration:', err));
  }, []);

  const handleWifiSubmit = (e) => {
    e.preventDefault();
    fetch('/api/wifi', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ ssid, password })
    })
      .then(res => {
        if (!res.ok) throw new Error('Fehler beim Speichern der WLAN-Daten');
        return res.json();
      })
      .then(() => alert('WLAN-Konfiguration gespeichert.'))
      .catch(err => alert('Fehler: ' + err.message));
  };

  const handleI2cSubmit = (e) => {
    e.preventDefault();
    fetch('/api/i2c', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ sda: parseInt(i2cSda), scl: parseInt(i2cScl) })
    })
      .then(res => {
        if (!res.ok) throw new Error('Fehler beim Speichern der I2C-Pins');
        return res.json();
      })
      .then(() => alert('I2C-Konfiguration gespeichert.'))
      .catch(err => alert('Fehler: ' + err.message));
  };

  return (
    <div className="card mb-5" id="basic-settings">
      <div className="card-header">Grundlegende Einstellungen</div>
      <div className="card-body">
        <form onSubmit={handleWifiSubmit}>
          <div className="mb-3">
            <label htmlFor="wifi-ssid" className="form-label">WLAN‑SSID</label>
            <input
              id="wifi-ssid"
              type="text"
              className="form-control"
              placeholder="SSID eingeben"
              value={ssid}
              onChange={(e) => setSsid(e.target.value)}
            />
          </div>

          <div className="mb-3">
            <label htmlFor="wifi-pass" className="form-label">WLAN‑Passwort</label>
            <input
              id="wifi-pass"
              type="password"
              className="form-control"
              placeholder="Passwort eingeben"
              value={password}
              onChange={(e) => setPassword(e.target.value)}
            />
          </div>

          <button type="submit" className="btn btn-primary">WLAN speichern</button>
        </form>

        <hr />

        <form onSubmit={handleI2cSubmit}>
          <div className="mb-3">
            <label htmlFor="i2c-sda" className="form-label">I2C SDA Pin</label>
            <select
              id="i2c-sda"
              className="form-select"
              value={i2cSda}
              onChange={(e) => setI2cSda(e.target.value)}
            >
              <option value="">-- auswählen --</option>
              {availablePins.map((pin) => (
                <option key={pin.value} value={pin.value}>{pin.label}</option>
              ))}
            </select>
          </div>

          <div className="mb-3">
            <label htmlFor="i2c-scl" className="form-label">I2C SCL Pin</label>
            <select
              id="i2c-scl"
              className="form-select"
              value={i2cScl}
              onChange={(e) => setI2cScl(e.target.value)}
            >
              <option value="">-- auswählen --</option>
              {availablePins.map((pin) => (
                <option key={pin.value} value={pin.value}>{pin.label}</option>
              ))}
            </select>
          </div>

          <button type="submit" className="btn btn-primary">I2C speichern</button>
        </form>
      </div>
    </div>
  );
}

export default Settings;
