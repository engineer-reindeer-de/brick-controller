// src/data/widgetSchemas.js
export const widgetSchemas = {
    button: {
      label: 'Button',
      options: [
        { name: 'label', label: 'Anzeigetext', type: 'string', default: 'Button' },
        { name: 'pin', label: 'Pin', type: 'integer', default: 5 }
      ]
    },
    slider: {
      label: 'Slider',
      options: [
        { name: 'label', label: 'Bezeichnung', type: 'string', default: 'LED Helligkeit' },
        { name: 'pin', label: 'Pin', type: 'integer', default: 15 }
      ]
    },
    joystick: {
      label: 'Joystick',
      options: [
        { name: 'xPin', label: 'X-Pin', type: 'integer', default: 12 },
        { name: 'yPin', label: 'Y-Pin', type: 'integer', default: 13 },
        { name: 'xInverted', label: 'X-Interierung', type: 'boolean', default: false },
        { name: 'yInverted', label: 'Y-Interierung', type: 'boolean', default: false },
        { name: 'snapback', label: 'Rückstellung', type: 'boolean', default: true },
        { name: 'xDrift', label: 'X-Korrektur', type: 'int', default: 0 },
        { name: 'yDrift', label: 'Y-Korrektur', type: 'int', default: 0 }
      ]
    },
    video: {
      label: 'Video Stream',
      options: [
        { name: 'url', label: 'StreamURL', type: 'string', default: '/stream' }
      ]
    },
    sensor: {
      label: 'Sensor',
      options: [
        { name: 'label', label: 'Bezeichnung', type: 'string', default: 'Temperatur' },
        { name: 'source', label: 'Quelle', type: 'string', default: '' }
      ]
    }
  };