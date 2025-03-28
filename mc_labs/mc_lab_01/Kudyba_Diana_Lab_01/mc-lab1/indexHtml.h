const char webPage[] PROGMEM = R"rawliteral(
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>LED Control</title>
        <style>
            body {
                font-family: Arial, sans-serif;
                text-align: center;
                background-color: #f4f4f4;
                padding: 20px;
            }
            .container {
                max-width: 400px;
                margin: auto;
                background: white;
                padding: 20px;
                box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
                border-radius: 8px;
            }
            h1 {
                color: #333;
            }
            button {
                background: #28a745;
                color: white;
                border: none;
                padding: 15px;
                font-size: 18px;
                cursor: pointer;
                margin: 10px;
                width: 100%;
                border-radius: 5px;
            }
            button:hover {
                background: #218838;
            }
        </style>
    </head>
    <body>
        <div class="container">
            <h1>ESP8266 LED Control</h1>
            <button onclick="toggleLED()">Toggle LEDs</button>
        </div>
    
        <script>
            function toggleLED() {
                fetch("/toggle").then(response => console.log("Toggled"));
            }
        </script>
    </body>
    </html>
    )rawliteral";
    