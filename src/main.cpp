#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <VL53L0X.h>

// -- NETWORK AND SERVER CONFIGURATIONS -- //
#define WIFI_SSID       "MAMBEE"
#define WIFI_PASSWORD   "****YOUR_WIFI_PASSWORD****"
#define SERVER_URL      "https://****YOUR_SERVER_URL****/silo/reading"
#define SILO_ID         "****YOUR_SILO_ID****"

// -- Hardware Definitions -- //
#define I2C_SDA_PIN     21
#define I2C_SCL_PIN     22
#define LED_GREEN_PIN   4 
#define LED_RED_PIN     2

// -- Root Certificate for HTTPS -- //
const char* rootCACertificate = \
"-----BEGIN CERTIFICATE-----\n" \
"-----END CERTIFICATE-----\n";

// -- Sensor Instance -- //
VL53L0X distanceSensor;

// -- LED Configuration -- //
void blink_led(uint8_t led_pin, int times, int delay_ms) {
    for (int i = 0; i < times; i++) {
        digitalWrite(led_pin, HIGH);
        delay(delay_ms);
        digitalWrite(led_pin, LOW);
        delay(delay_ms);
    }
}

void setup_leds() {
    pinMode(LED_GREEN_PIN, OUTPUT);
    pinMode(LED_RED_PIN, OUTPUT);
    digitalWrite(LED_GREEN_PIN, LOW);
    digitalWrite(LED_RED_PIN, LOW);
}

// -- Initialize VL53L0X Sensor with Library -- //
bool init_distance_sensor() {
    Serial.println("Iniciando sensor VL53L0X com biblioteca...");
    
    // Configure sensor timeout
    distanceSensor.setTimeout(500);
    
    // Initialize sensor
    if (!distanceSensor.init()) {
        Serial.println("❌ Falha ao inicializar o sensor VL53L0X!");
        return false;
    }
    
    // Optional: Configure for long range
    // distanceSensor.setSignalRateLimit(0.1);
    // distanceSensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
    // distanceSensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
    
    // Start continuous measurements
    distanceSensor.startContinuous();
    
    Serial.println("✅ Sensor VL53L0X inicializado com sucesso");
    return true;
}

// -- Function to send distance data via HTTPS POST -- //
void post_distance_to_server(int distance_mm) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Erro: Wi-Fi desconectado");
        blink_led(LED_RED_PIN, 2, 200);
        return;
    }

    WiFiClientSecure *client = new WiFiClientSecure;
    HTTPClient https;
    
    // Configure SSL client
    // client->setCACert(rootCACertificate);
    // For testing with self-signed certificates, use:
    client->setInsecure(); // ⚠️ Remove this in production!

    // Create JSON payload - CORRIGIDO: silo_id e level_value estavam trocados
    String jsonPayload = "{\"silo_id\":\"" + String(SILO_ID) + "\"," + 
                         "\"level_value\":" + String(distance_mm) + "}";

    Serial.println("Enviando POST para: " + String(SERVER_URL));
    Serial.println("Payload: " + jsonPayload);

    // Start HTTPS connection
    if (https.begin(*client, SERVER_URL)) {
        https.addHeader("Content-Type", "application/json");
        https.addHeader("Connection", "close");

        // Send POST request
        int httpResponseCode = https.POST(jsonPayload);

        // Check response
        if (httpResponseCode > 0) {
            Serial.print("Resposta HTTP: ");
            Serial.println(httpResponseCode);
            
            String response = https.getString();
            Serial.print("Resposta do servidor: ");
            Serial.println(response);
            
            if (httpResponseCode == 200) {
                blink_led(LED_GREEN_PIN, 1, 300); // Success indicator
            } else {
                blink_led(LED_RED_PIN, 1, 300); // Warning indicator
            }
        } else {
            Serial.print("Erro na requisição: ");
            Serial.println(https.errorToString(httpResponseCode));
            blink_led(LED_RED_PIN, 2, 200); // Error indicator
        }

        https.end();
    } else {
        Serial.println("Erro ao inici conexão HTTPS");
        blink_led(LED_RED_PIN, 3, 150);
    }
    
    delete client;
}

// -- Wi-Fi Connection -- //
bool connect_wifi() {
    Serial.println("Conectando ao Wi-Fi '" + String(WIFI_SSID) + "'...");
    
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(1000);
        Serial.print(".");
        attempts++;
        blink_led(LED_GREEN_PIN, 1, 100); // Blink during connection attempt
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConectado com sucesso!");
        Serial.print("IP do ESP32: ");
        Serial.println(WiFi.localIP());
        blink_led(LED_GREEN_PIN, 3, 250);
        return true;
    } else {
        Serial.println("\nFalha ao conectar ao Wi-Fi");
        blink_led(LED_RED_PIN, 5, 200);
        return false;
    }
}

// -- Read distance from sensor with error handling -- //
int read_sensor_distance() {
    uint16_t distance = distanceSensor.readRangeContinuousMillimeters();
    
    if (distanceSensor.timeoutOccurred()) {
        Serial.println("⚠️  Timeout na leitura do sensor");
        return -1; // Error code for timeout
    }
    
    if (distance >= 8190) {
        Serial.println("⚠️  Sensor fora do alcance máximo");
        return -2; // Error code for out of range
    }
    
    return distance;
}

void setup() {
    Serial.begin(9600);
    delay(3000); // Wait for serial monitor
    
    Serial.println("Iniciando sistema de monitoramento do silo...");
    
    // Initialize LEDs
    setup_leds();
    
    // Initialize I2C for sensor
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    Wire.setClock(400000); // 400kHz
    
    // Connect to Wi-Fi
    if (!connect_wifi()) {
        Serial.println("Sistema parado devido à falha de conexão Wi-Fi");
        while(1) {
            blink_led(LED_RED_PIN, 3, 500); // Continuous error indication
            delay(2000);
        }
    }
    
    // Initialize distance sensor with library
    Serial.println("Iniciando sensor VL53L0X...");
    if (init_distance_sensor()) {
        Serial.println("Sensor VL53L0X inicializado com sucesso");
        blink_led(LED_GREEN_PIN, 2, 200);
    } else {
        Serial.println("Falha ao inicializar o sensor VL53L0X");
        blink_led(LED_RED_PIN, 4, 200);
        // Continue anyway for testing? Or halt system?
        // while(1) { delay(1000); } // Uncomment to halt on sensor failure
    }
    
    Serial.println("\nSistema pronto para operação");
    Serial.println("=== Iniciando loop principal ===");
}

void loop() {
    static unsigned long lastReading = 0;
    const unsigned long readingInterval = 10000; // 10 seconds
    
    // Check Wi-Fi connection
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Wi-Fi desconectado - tentando reconectar...");
        if (!connect_wifi()) {
            delay(5000);
            return;
        }
    }
    
    // Read sensor every 10 seconds
    if (millis() - lastReading >= readingInterval) {
        lastReading = millis();
        
        int distance = read_sensor_distance();
        
        // Process and send reading
        if (distance > 0 && distance < 8190) {
            Serial.println("{\"origem\": \"" + String(SILO_ID) + "\", \"distancia\": " + 
                          String(distance) + ", \"status\": \"ok\"}");
            
            post_distance_to_server(distance);
            blink_led(LED_GREEN_PIN, 1, 100); // Quick success blink
        } else {
            // Handle different error cases
            String errorMsg = "{\"origem\": \"" + String(SILO_ID) + "\", \"distancia\": ";
            if (distance == -1) {
                errorMsg += "null, \"status\": \"timeout\"}";
            } else if (distance == -2) {
                errorMsg += "null, \"status\": \"out_of_range\"}";
            } else {
                errorMsg += String(distance) + ", \"status\": \"erro\"}";
            }
            Serial.println(errorMsg);
            
            blink_led(LED_RED_PIN, 2, 200); // Error indication
            
            // Optional: Try to reinitialize sensor on persistent errors
            static int errorCount = 0;
            errorCount++;
            if (errorCount >= 3) {
                Serial.println("Muitos erros consecutivos - tentando reinicializar sensor...");
                init_distance_sensor();
                errorCount = 0;
            }
        }
        
        Serial.println("--- Próxima leitura em 10 segundos ---");
    }
    
    // Small delay to prevent watchdog issues, but allow sensor continuous mode to work
    delay(100);
}