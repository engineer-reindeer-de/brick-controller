#include "esp_camera.h"
#include <ESPAsyncWebServer.h>
// -------- Asynchrones MJPEG‑Streaming ohne Watchdog‑Probleme --------
#define PART_BOUNDARY "123456789000000000000987654321"
static const char *STREAM_CONTENT_TYPE = "multipart/x-mixed-replace; boundary=" PART_BOUNDARY;
static const char *STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char *STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

class AsyncJpegStreamResponse : public AsyncAbstractResponse
{
private:
  camera_fb_t *_fb = nullptr;
  size_t _idx = 0;
  size_t _len = 0;
  uint64_t _last = 0;

public:
  AsyncJpegStreamResponse()
  {
    _code = 200;
    _contentType = STREAM_CONTENT_TYPE;
    _sendContentLength = false;
    _chunked = true;
  }

  ~AsyncJpegStreamResponse() override
  {
    if (_fb)
      esp_camera_fb_return(_fb);
  }

  bool _sourceValid() const override { return true; }

  size_t _fillBuffer(uint8_t *buf, size_t maxLen) override
  {
    size_t written = 0;

    // Neues Frame holen, wenn keines aktiv ist
    if (!_fb)
    {
      if (maxLen < strlen(STREAM_BOUNDARY) + 64)
        return RESPONSE_TRY_AGAIN;

      _fb = esp_camera_fb_get();
      if (!_fb)
        return RESPONSE_TRY_AGAIN;

      _idx = 0;
      _len = _fb->len;

      // Boundary + Header ausgeben
      written += snprintf((char *)(buf + written), // schreibe direkt in Puffer
                          maxLen - written,        // verbleibende Größe
                          "%s"
                          "Content-Type: image/jpeg\r\n"
                          "Content-Length: %u\r\n\r\n",
                          STREAM_BOUNDARY, _len);
    }

    // Bilddaten kopieren
    size_t toCopy = std::min(maxLen - written, _len - _idx);
    memcpy(buf + written, _fb->buf + _idx, toCopy);
    written += toCopy;
    _idx += toCopy;

    // Frame komplett? → zurückgeben und Stats ausgeben
    if (_idx == _len)
    {
      size_t thisLen = _len;
      esp_camera_fb_return(_fb);
      _fb = nullptr;
      _idx = _len = 0;
    }

    return written;
  }
};