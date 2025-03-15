#ifndef INDEX_HTML_H
#define INDEX_HTML_H

const char htmlPage[] PROGMEM = R"rawliteral(
<html>
  <body>
    <h1>LED Control</h1>
    <button ondblclick="toggle()">Double Click to Toggle LEDs</button>
    <p>Status: <span id="status">Checking...</span></p>
    <script>
      function toggle() {
        fetch('/toggle').then(res => res.text()).then(data => {
          document.getElementById('status').innerText = data;
        });
      }
      function updateStatus() {
        fetch('/status').then(res => res.text()).then(data => {
          document.getElementById('status').innerText = data;
          setTimeout(updateStatus, 2000);
        });
      }
      updateStatus();
    </script>
  </body>
</html>
)rawliteral";

#endif
