import React, { useEffect, useRef, useState, useCallback } from 'react';
import nipplejs from 'nipplejs';

function Joystick({ xPin, yPin, snapback, drift, xInverted, yInverted, xDrift, yDrift }) {
  const joystickRef = useRef(null);
  const managerRef = useRef(null);

  const [availableChannels, setAvailableChannels] = useState([]);

  useEffect(() => {
    fetch('/api/pwm_channels')
      .then(res => res.json())
      .then(data => setAvailableChannels(data))
      .catch(err => console.error('Fehler beim Laden der PWM-Kanäle:', err));
  }, []);

  useEffect(() => {
    if (joystickRef.current && !managerRef.current) {
      managerRef.current = nipplejs.create({
        zone: joystickRef.current,
        mode: 'static',
        position: { left: '50%', top: '50%' },
        color: 'blue',
        size: 150,
        restJoystick: snapback,
        restOpacity: 0.5,
      });

      let lastSend = 0;
      const throttleMs = 100; // Max. alle 100ms senden

      managerRef.current.on('move', (_, data) => {
        const now = Date.now();
        if (now - lastSend < throttleMs) return;
        lastSend = now;

        if (data && data.vector) {
          const x = Math.round((((xInverted ? -1 : 1) * data.vector.x) + (xDrift || 0)) * 100);
          const y = Math.round((((yInverted ? -1 : 1) * data.vector.y) + (yDrift || 0)) * 100);
          console.log(`Joystick move: x=${x}, y=${y}, Pins: ${xPin}, ${yPin}`);

          if (xPin !== undefined) {
            const msg = JSON.stringify({
              target: { type: "pwm", chip: "esp32", pin: xPin },
              value: Math.min(255, Math.max(0, x + 127))
            });
            window.socket?.send(msg);
          }

          if (yPin !== undefined) {
            const msg = JSON.stringify({
              target: { type: "pwm", chip: "esp32", pin: yPin },
              value: Math.min(255, Math.max(0, y + 127))
            });
            window.socket?.send(msg);
          }
        }
      });

      managerRef.current.on('end', () => {
        if (snapback) {
          console.log('Joystick released to center');
          // Reset Werte auf 127 senden (Mittelstellung)
          if (xPin !== undefined) {
            const msg = JSON.stringify({
              target: { type: "pwm", chip: "esp32", pin: xPin },
              value: 127
            });
            window.socket?.send(msg);
          }
          if (yPin !== undefined) {
            const msg = JSON.stringify({
              target: { type: "pwm", chip: "esp32", pin: yPin },
              value: 127
            });
            window.socket?.send(msg);
          }
        }
      });
    }

    return () => {
      if (managerRef.current) {
        managerRef.current.destroy();
        managerRef.current = null;
      }
    };
  }, [xPin, yPin, snapback, drift, xInverted, yInverted, xDrift, yDrift]);

  return (
    <div className="card mb-3">
      <div className="card-header">Joystick</div>
      <div className="card-body text-center">
        <div ref={joystickRef} style={{ width: '100%', height: '200px' }} />
      </div>
    </div>
  );
}

export default Joystick;