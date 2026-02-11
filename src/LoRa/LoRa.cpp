#include "LoRa/LoRa.h"

int pixelsInText;
int x;
int y;
byte velocidad;
unsigned long t_texto;

String msg_screen = "Example test";

float ph;
String sender_id;
int turbidity;
String sender_type;
int temperature;
int rssi;
int snr;

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(ANCHO, ALTO, PIN,
  NEO_MATRIX_BOTTOM  + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800
);

void ledPanelBegin() {
  matrix.begin();  //inicia la pantalla
  matrix.setTextWrap(false);  //ajuste de texto
  matrix.setBrightness(BRILLO);  //asigna  el valor del brillo a la pantalla
}
// Creación del objeto display para control del OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

// Función para inicializar el módulo LoRa
void loraBegin() {
  int counter = 0; // Contador para intentos de inicio

  // Configura los pines SPI para LoRa
  SPI.begin(SCK, MISO, MOSI, SS);
  // Configura el módulo transceptor LoRa
  LoRa.setSyncWord(0x7E);//red
  LoRa.setPins(SS, RST, DIO0);

  // Intenta iniciar LoRa. Repite hasta que lo logre o hasta un máximo de 10 intentos
  while (!LoRa.begin(BAND) && counter < 10) {
    Serial.print("."); // Imprime un punto como indicador de progreso
    counter++; // Incrementa el contador de intentos
    delay(500); // Espera medio segundo entre intentos
  }

  // Verifica si alcanzó el máximo de intentos sin éxito
  if (counter == 10) {
    Serial.println("¡Inicio de LoRa fallido!");
  } else {
    Serial.println("Inicialización de LoRa OK!");
  }

  delay(2000); // Espera 2 segundos antes de continuar

  loraOled(); // Inicia la configuración y muestra inicial en el display OLED.
}

// Manejar mensajes de LoRa.
void processPackets() {

  String lora_data;
  // Si hay datos disponibles después de la espera, léelos
  if (LoRa.available()) {
    
    Serial.println("Paquete LoRa recibido.");
    // Mostrar la RSSI "Received Signal Strength Indication" del paquete recibido.
    Serial.println("RSSI: " + String(LoRa.packetRssi()));
    // Mostrar la SNR "Signal-to-Noise Ratio" del paquete recibido.
    Serial.println("SNR: " + String(LoRa.packetSnr()));

    lora_data = LoRa.readString();

    // Verificar si el tipo de remitente es "bouy" (boya)
    if (lora_data.indexOf("\"sender_type\":\"bouy\"") != -1) {
      // Ejecutar la función que maneja el mensaje si es una boya
      Serial.println("Mensaje recibido de un remitente tipo boya:");

      Serial.println(lora_data);
          
      // Crear un objeto JSON para analizar el mensaje recibido
      StaticJsonDocument<200> jsonDocument;
      deserializeJson(jsonDocument, lora_data);

      // Extraer los valores del objeto JSON
      const char* rq_sender_type = jsonDocument["sender_type"];
      const char* rq_sender_id = jsonDocument["sender_id"];
      int rq_turbidity = jsonDocument["sensor_info"]["turbidity"];
      int rq_temperature = jsonDocument["sensor_info"]["temperature"];
      float rq_ph = jsonDocument["sensor_info"]["pH"];

      // Asignar los valores a las variables definidas
      ph = rq_ph;
      sender_id = rq_sender_id;
      turbidity = rq_turbidity;
      sender_type = rq_sender_type;
      temperature = rq_temperature;
      rssi = LoRa.packetRssi();
      snr = LoRa.packetSnr();
    }
    // Verificar si el tipo de remitente es "turret" (torreta)
    else if (lora_data.indexOf("\"sender_type\":\"turret\"") != -1) {
      // Ejecutar la función que maneja el mensaje si es una torreta
      Serial.println("Mensaje recibido de un remitente tipo torreta:");
      // manageTurretData(lora_data);
      // Serial.println(lora_data);
    } else {
      Serial.println("Mensaje recibido de un remitente no autorizado.");
    }
  }
}

String handleWaterRequest() {
  // Crear un objeto JSON
	DynamicJsonDocument doc(200);

	doc["sender_type"] = sender_type;
	doc["sender_id"] = sender_id;
	doc["ph"] = ph;
	doc["temperature"] = temperature;
	doc["turbidity"] = turbidity;

	String response;
	serializeJson(doc, response);
	
	return response;
}


void ledCarousel() {
  matrix.fillScreen(matrix.Color(BKRED, BKGREEN, BKBLUE));
  
  matrix.setBrightness(BRILLO);

  if(millis() > t_texto + 10){
    matrix.fillScreen(0);

    // Crear el mensaje dinámico con los valores de los sensores y los colores correspondientes
    String temp_msg = "Temperatura: " + String(temperature);
    String ph_msg = ", PH: " + String(ph);
    String turb_msg = ", Turbidez: " + String(turbidity);

    // Colores por defecto para cada parte del mensaje
    uint32_t temp_color = matrix.Color(RED, GREEN, BLUE);
    uint32_t ph_color = matrix.Color(RED, GREEN, BLUE);
    uint32_t turb_color = matrix.Color(RED, GREEN, BLUE);

    // Cambiar el color del texto en función del estado de las variables de los sensores
    if (temperature >= TEMP_MIN && temperature <= TEMP_MAX) {
      temp_color = matrix.Color(0, 255, 0); // Verde si la temperatura está dentro del rango apropiado
    }
    if (ph >= PH_MIN && ph <= PH_MAX) {
      ph_color = matrix.Color(0, 255, 0); // Verde si el pH está dentro del rango apropiado
    }
    if (turbidity >= TURB_MIN && turbidity <= TURB_MAX) {
      turb_color = matrix.Color(0, 255, 0); // Verde si la turbidez está dentro del rango apropiado
    }

    // Concatenar las partes del mensaje con los colores correspondientes
    String dynamic_msg = temp_msg + ph_msg + turb_msg;

    // Calcular la longitud total del mensaje dinámico
    pixelsInText = dynamic_msg.length() * 6; // 6 es el ancho de cada carácter

    if(--x < -pixelsInText) {
      x = matrix.width();
    }

    // Mostrar el mensaje dinámico en la matriz con los colores correspondientes
    matrix.setTextColor(temp_color);
    matrix.setCursor(x, 0);
    matrix.print(temp_msg);

    matrix.setTextColor(ph_color);
    matrix.print(ph_msg);

    matrix.setTextColor(turb_color);
    matrix.print(turb_msg);

    matrix.show();
    t_texto = millis();
  }
}

// Inicialización del módulo OLED
void loraOled() {
  // Reinicio programático del display OLED
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  // Inicia la comunicación I2C y verifica la inicialización del display
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) {
    Serial.println(F("La asignación del SSD1306 falló"));
    for(;;); // Bucle infinito en caso de fallo
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("LORA RECIEVER");
  display.setCursor(0,10);
  display.print("LoRa Initialized OK!");
  display.display();
}
