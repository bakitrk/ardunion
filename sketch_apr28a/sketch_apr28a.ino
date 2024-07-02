void setup() {
  // put your setup code here, to run once:
#include <ESP8266WiFi.h>
#include <WiFiEsp.h>
}

void loop() {
  // put your main code here, to run repeatedly:
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DS3231.h>
#include <SoftwareSerial.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

const char* ssid = "kayseri"; // WiFi ağınızın adını ve şifresini girin
const char* password = "100646";
const char* mqtt_server = "1.1.1.1"; // MQTT brokerinizi IP adresi olarak girin
const char* mqtt_username = "baki"; // MQTT brokerinizin kullanıcı adını ve şifresini girin
const char* mqtt_password = "2050";

WiFiClient espClient;
PubSubClient client(espClient);

DS3231 rtc(SDA, SCL);
LiquidCrystal_I2C lcd(0x27, 16, 2);

int alarmSaat = 7; // Alarm saati, başlangıçta varsayılan olarak 07:00

void setup_wifi() {
  delay(10);
  // WiFi ağına bağlanma
  Serial.println();
  Serial.print("WiFi'ye bağlanılıyor: ");
  Serial.println(ssid);

  WiFi.begin(ssid, 100646);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi'ye bağlandı");
  Serial.println("IP adresi: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  // MQTT abonelikleri burada işlenir
  if (String(topic) == "alarm/time") {
    // Gelen zamanı ayarla
    String zaman = "";
    for (int i = 0; i < length; i++) {
      zaman += (char)payload[i];
    }
    alarmSaat = zaman.toInt();
  }
}

void reconnect() {
  // Loop içinde MQTT bağlantısını yeniden kurar
  while (!client.connected()) {
    Serial.print("MQTT Broker'ına bağlanılıyor...");
    if (client.connect("ArduinoClient", mqtt_username, mqtt_password)) {
      Serial.println("başarılı!");
      client.subscribe("alarm/time"); // Alarm ayarlamak için bu topiği abone edin
    } else {
      Serial.print("Hata, tekrar denenecek: ");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  rtc.begin();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Calar Saat");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  DateTime now = rtc.now();
  int saat = now.hour();
  int dakika = now.minute();
  lcd.setCursor(0, 1);
  lcd.print("Saat: ");
  if (saat < 10) {
    lcd.print("0");
  }
  lcd.print(saat);
  lcd.print(":");
  if (dakika < 10) {
    lcd.print("0");
  }
  lcd.print(dakika);
  
  // Alarmı kontrol et
  if (saat == alarmSaat && dakika == 0) { // Alarmı belirtilen saatte çal
    alarm();
  }
  
  delay(1000);
}

void alarm() {
  // Alarm sesini buraya ekleyebilirsiniz, buzzer veya hoparlör kullanabilirsiniz
  lcd.setCursor(0, 1);
  lcd.print("ALARM!");
  delay(5000); // Alarmı 5 saniye boyunca çal
}

}
