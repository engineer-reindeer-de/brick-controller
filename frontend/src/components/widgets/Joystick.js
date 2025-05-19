import React, { useLayoutEffect, useRef, useState } from 'react';
import nipplejs from 'nipplejs';

function Joystick({ xPin, yPin, snapback, drift, xInverted, yInverted, xDrift, yDrift, availablePins }) {
  const joystickRef = useRef(null);
  const managerRef = useRef(null);
  const [isVisible, setIsVisible] = useState(false);

  useLayoutEffect(() => {
    const observer = new window.ResizeObserver((entries) => {
      for (let entry of entries) {
        if (entry.contentRect.width > 0 && entry.contentRect.height > 0) {
          setIsVisible(true);
        }
      }
    });

    if (joystickRef.current) {
      observer.observe(joystickRef.current);
    }

    return () => {
      observer.disconnect();
    };
  }, []);

  useLayoutEffect(() => {
    if (isVisible && joystickRef.current && !managerRef.current) {
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
      const throttleMs = 100;

      managerRef.current.on('move', (_, data) => {
        const now = Date.now();
        if (now - lastSend < throttleMs) return;
        lastSend = now;

        if (data && data.vector) {
          const driftX = Number(xDrift || 0);
          const driftY = Number(yDrift || 0);

          const rawX = ((data.vector.x + driftX) * (xInverted ? -1 : 1)) * 100;
          const rawY = ((data.vector.y + driftY) * (yInverted ? -1 : 1)) * 100;

          const x = Math.round(Math.max(-100, Math.min(100, rawX)));
          const y = Math.round(Math.max(-100, Math.min(100, rawY)));

          console.log(`Joystick raw: x=${rawX}, y=${rawY} → clamped: x=${x}, y=${y}`);

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
  }, [isVisible, xPin, yPin, snapback, drift, xInverted, yInverted, xDrift, yDrift]);

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