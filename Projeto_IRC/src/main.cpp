#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WebSocketsServer.h>

const char* ssid = "projeto8266";
const char* password = "projetocir";

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

const int relayPinT03 = 5;   // Pino do ESP8266 conectado ao relé da máquina T03
const int relayPinT04 = 4;
bool relayStateT03 = false;
bool relayStateT04 = false;

void handleRelayControl();

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-br">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Interface de Controle</title>
    <style>
        body {
            margin: 0;
            padding: 0;
            height: 100vh;
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
            font-family: 'Arial', sans-serif;
            color: #fff; /* Texto branco */
            background-image: url('https://images.wallpaperscraft.com/image/single/dark_spots_texture_50355_1920x1080.jpg');
        }

        .container {
            display: flex;
            flex-wrap: wrap;
            gap: 20px;
            justify-content: center;
            margin-top: 20px; /* Espaçamento superior */
        }

        .main-button {
            padding: 15px 30px;
            font-size: 18px;
            font-weight: bold;
            color: #fff;
            border: none;
            border-radius: 8px;
            cursor: pointer;
            background: linear-gradient(45deg, #2c3e50, #34495e); /* Gradiente azul escuro */
            transition: transform 0.3s ease;
            min-width: 120px;
        }

        .main-button:hover {
            transform: scale(1.05);
        }

        .popup {
            display: none;
            position: fixed;
            top: 50%;
            left: 50%;
            width: 80%;
            transform: translate(-50%, -50%);
            max-width: 600px;
            background-color: #1f1f1f;
            z-index: 1000;
            border-radius: 10px;
            text-align: center;
            box-shadow: 0 2px 10px rgba(0, 0, 0, 0.3); /* Sombra suave */
            color: #fff;
            align-items: center;
            justify-content: center;
        }

        .popup-content {
            padding: 40px;
            border-radius: 10px;
            text-align: center;
            position: relative;
        }

        .close-button {
            position: absolute;
            top: 0;
            right: 0;
            cursor: pointer;
            color: #ccc;
            font-size: 24px;
            background: none;
            border: none;
            z-index: 2;
        }

        .close-button:hover {
            color: #fff;
        }

        .inner-button {
            padding: 10px 20px;
            font-size: 14px;
            color: #fff;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            margin: 5px;
            background-color: #dc3545; /* Vermelho inicial */
            transition: background-color 0.3s ease;
        }

        .inner-button.on {
            background-color: #28a745; /* Verde */
        }

        .inner-button.off {
            background-color: #dc3545; /* Vermelho */
        }

        .titulo {
            margin-bottom: 20px; /* Espaçamento inferior do título */
        }

        .logosenai{
        position: absolute;
        top: 10px; 
        right: 10px; 
        width: 150px; 
        height: auto; 
        }

    </style>
</head>
<body>
    <img class="logosenai" src="https://cdn.discordapp.com/attachments/656942253008355348/1254568188276899967/firjanlogo1.png?ex=6679f72c&is=6678a5ac&hm=03bb4c2941388fc100523608087ed84082197d63e0c1a7a03508389e82c83abf&">
    <h1 class="titulo">Interface de Controle</h1>  
    <div class="container">
        <button id="areaDeTornosButton" class="main-button">Tornos</button>
        <button id="areaDeFresasButton" class="main-button">Fresadoras</button>
        <button id="areaDeCncButton" class="main-button">CNC</button>

        <div id="tornosPopup" class="popup">
            <div class="popup-content">
                <button class="close-button" onclick="closePopup()">x</button>
                <button id="t01Button" class="inner-button off" onclick="toggleTorno('t1')">T01</button>
                <button id="t02Button" class="inner-button off" onclick="toggleTorno('t02')">T02</button>
                <button id="t03Button" class="inner-button off" onclick="toggleTorno('t03')">T03</button>
                <button id="t04Button" class="inner-button off" onclick="toggleTorno('t04')">T04</button>
                <button id="t05Button" class="inner-button off" onclick="toggleTorno('t05')">T05</button>
                <button id="t06Button" class="inner-button off" onclick="toggleTorno('t06')">T06</button>
                <button id="t07Button" class="inner-button off" onclick="toggleTorno('t07')">T07</button>
                <button id="t08Button" class="inner-button off" onclick="toggleTorno('t08')">T08</button>
                <button id="t09Button" class="inner-button off" onclick="toggleTorno('t09')">T09</button>
                <button id="t19Button" class="inner-button off" onclick="toggleTorno('t010')">T10</button>
            </div>
        </div>

        <div id="fresasPopup" class="popup">
            <div class="popup-content">
                <button class="close-button" onclick="closePopup()">x</button>
                <button id="f01Button" class="inner-button off" onclick="toggleFresa('f01')">F01</button>
                <button id="f02Button" class="inner-button off" onclick="toggleFresa('f02')">F02</button>
                <button id="f03Button" class="inner-button off" onclick="toggleFresa('f03')">F03</button>
                <button id="f04Button" class="inner-button off" onclick="toggleFresa('f04')">F04</button>
            </div>
        </div>

        <div id="cncPopup" class="popup">
            <div class="popup-content">
                <button class="close-button" onclick="closePopup()">x</button>
                <button id="c01Button" class="inner-button off" onclick="toggleCnc('c01')">C01</button>
                <button id="c02Button" class="inner-button off" onclick="toggleCnc('c02')">C02</button>
                <button id="c03Button" class="inner-button off" onclick="toggleCnc('c03')">C03</button>
                <button id="c04Button" class="inner-button off" onclick="toggleCnc('c04')">C04</button>
            </div>
        </div>
    </div>

    <script>
        const ws = new WebSocket(`ws://${window.location.hostname}:81/`);

        ws.onmessage = (event) => {
            const data = JSON.parse(event.data);
            const button = document.getElementById(data.id + 'Button');
            if (button) {
                button.classList.toggle('on', data.state);
                button.classList.toggle('off', !data.state);
            }
        };

        const tornosPopup = document.getElementById('tornosPopup');
        const fresasPopup = document.getElementById('fresasPopup');
        const cncPopup = document.getElementById('cncPopup');

        document.getElementById('areaDeTornosButton').addEventListener('click', () => {
            tornosPopup.style.display = 'flex';
        });

        document.getElementById('areaDeFresasButton').addEventListener('click', () => {
            fresasPopup.style.display = 'flex';
        });

        document.getElementById('areaDeCncButton').addEventListener('click', () => {
            cncPopup.style.display = 'flex';
        });


        function closePopup() {
            tornosPopup.style.display = 'none';
            fresasPopup.style.display = 'none';
            cncPopup.style.display = 'none';
        }

        function toggleTorno(tornoId) {
            const button = document.getElementById(tornoId + 'Button');
            const isOn = button.classList.contains('on');

            fetch('/toggle_' + tornoId + '_rele')
                .then(response => {
                    if (response.ok) {
                        button.classList.toggle('on', !isOn);
                        button.classList.toggle('off', isOn);
                        ws.send(JSON.stringify({ id: tornoId, state: !isOn }));
                    }
                })
                .catch(error => console.error('Erro ao mudar estado do relé:', error));
        }

        function updateButtonStates() {
            const tornoIds = ['t03', 't04'];

            tornoIds.forEach(tornoId => {
                fetch('/get_' + tornoId + '_state')
                    .then(response => response.json())
                    .then(data => {
                        const button = document.getElementById(tornoId + 'Button');
                        button.classList.toggle('on', data.state);
                        button.classList.toggle('off', !data.state);
                    })
                    .catch(error => console.error('Erro ao obter estado do torno ' + tornoId + ':', error));
            });
        }

        updateButtonStates();

        window.addEventListener('click', (event) => {
            if (event.target === tornosPopup || event.target === fresasPopup) {
                event.target.style.display = 'none';
            }
        });

        ws.onopen = () => {
            ws.send(JSON.stringify({ request: "syncState" }));
        };
    </script>
</body>
</html>
)rawliteral";

void handleRoot() {
    server.send(200, "text/html", index_html);
}

// Função para alternar o estado do relé
void toggleRelay(int pin, bool &state) {
    state = !state;
    digitalWrite(pin, state ? LOW : HIGH);
    String message = "{\"id\":\"" + String(pin == relayPinT03 ? "t03" : "t04") + "\",\"state\":" + String(state) + "}";
    webSocket.broadcastTXT(message);
}

// Função para enviar o estado dos relés ao cliente especificado
void sendRelayState(uint8_t num) {
    String message = "{\"id\":\"t03\",\"state\":" + String(relayStateT03) + "}";
    webSocket.sendTXT(num, message);
    message = "{\"id\":\"t04\",\"state\":" + String(relayStateT04) + "}";
    webSocket.sendTXT(num, message);
}

// Função de evento para o WebSocket
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    switch (type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED: {
            IPAddress ip = webSocket.remoteIP(num);
            Serial.printf("[%u] Connection from %s\n", num, ip.toString().c_str());
            sendRelayState(num);  // Envia o estado atual para o cliente recém-conectado
            break;
        }
        case WStype_TEXT:
            Serial.printf("[%u] Text: %s\n", num, payload);
            break;
        default:
            break;
    }
}

// Função de configuração
void setup() {
    Serial.begin(9600);

    pinMode(relayPinT03, OUTPUT);
    pinMode(relayPinT04, OUTPUT);
    digitalWrite(relayPinT03, HIGH);  // Inicialmente desliga o relé
    digitalWrite(relayPinT04, HIGH);

    WiFi.begin(ssid, password);
    Serial.print("Conectando ao WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }

    if (MDNS.begin("projetoirc")) {  
        Serial.println("mDNS responder iniciado");
    } else {
        Serial.println("Erro ao iniciar mDNS responder");
    }

    Serial.println();
    Serial.println("Conectado ao WiFi");
    Serial.print("Endereço IP: ");
    Serial.println(WiFi.localIP());

    server.on("/", HTTP_GET, handleRoot);
    server.on("/toggle_t03_rele", HTTP_GET, []() {
        toggleRelay(relayPinT03, relayStateT03);
        server.send(200, "text/plain", relayStateT03 ? "Relé ligado" : "Relé desligado");
    });

    server.on("/toggle_t04_rele", HTTP_GET, []() {
        toggleRelay(relayPinT04, relayStateT04);
        server.send(200, "text/plain", relayStateT04 ? "Relé ligado" : "Relé desligado");
    });

    server.begin();
    Serial.println("Servidor HTTP iniciado");

    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    Serial.println("Servidor WebSocket iniciado");

    MDNS.update();
}

// Função de loop
void loop() {
    server.handleClient();
    webSocket.loop();
    MDNS.update();
}

// Função para enviar o estado dos relés a todos os clientes
void sendRelayState() {
    String message = "{\"id\":\"t03\",\"state\":" + String(relayStateT03) + "}";
    webSocket.broadcastTXT(message);
    message = "{\"id\":\"t04\",\"state\":" + String(relayStateT04) + "}";
    webSocket.broadcastTXT(message);
}