import React from 'react';

function VideoStream({ url }) {
  return (
    <div className="card mb-3">
      <div className="card-header">Live-Stream</div>
      <div className="card-body text-center">
        <img src={url} alt="Live Stream" className="img-fluid rounded" />
      </div>
    </div>
  );
}

export default VideoStream;