#include <WiFi.h>
#include <WiFiClientSecure.h>

// -------------------
// CONFIG WIFI
// -------------------
const char* ssid = "plopplopplop";
const char* wifi_password = "papapapa";

// -------------------
// CONFIG Gmail SMTP
// -------------------
const char* smtp_server = "smtp.gmail.com";
const int smtp_port = 465;

const char* email_login = "olivierrenault75@gmail.com";
const char* email_password = "abnd khos zbab yxsk";   // mot de passe d'application Gmail

const char* email_from = "olivierrenault75@gmail.com";

// Tableau de destinataires
const char* recipients[] = {
  "olivierrenault75@gmail.com",
  "oliv_ren@yahoo.fr",
  "sophie.renault@yahoo.fr",
  "prenault@gmail.com",
  "baptisterenault09@gmail.com",
  "alexandrerenault0301@gmail.com"
};
const int nbRecipients = sizeof(recipients) / sizeof(recipients[0]);

// -------------------
// GPIO
// -------------------
int switches[] = {32, 33, 25, 26};
String lastMsg = "";

// ---------------------------------------------------------
// FONCTION BASE64
// ---------------------------------------------------------
String encodeBase64(const String &input) {
  const char base64_chars[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  String encoded = "";
  int i = 0;
  unsigned char char_array_3[3];
  int input_len = input.length();
  int pos = 0;

  while (input_len--) {
    char_array_3[i++] = input[pos++];
    if (i == 3) {
      encoded += base64_chars[(char_array_3[0] & 0xfc) >> 2];
      encoded += base64_chars[((char_array_3[0] & 0x03) << 4)
                           + ((char_array_3[1] & 0xf0) >> 4)];
      encoded += base64_chars[((char_array_3[1] & 0x0f) << 2)
                           + ((char_array_3[2] & 0xc0) >> 6)];
      encoded += base64_chars[char_array_3[2] & 0x3f];
      i = 0;
    }
  }

  if (i > 0) {
    for (int j = i; j < 3; j++) char_array_3[j] = '\0';
    encoded += base64_chars[(char_array_3[0] & 0xfc) >> 2];
    encoded += base64_chars[((char_array_3[0] & 0x03) << 4)
                         + ((char_array_3[1] & 0xf0) >> 4)];
    if (i == 1) encoded += "==";
    else if (i == 2) {
      encoded += base64_chars[((char_array_3[1] & 0x0f) << 2)
                           + ((char_array_3[2] & 0xc0) >> 6)];
      encoded += "=";
    }
  }

  return encoded;
}

// ---------------------------------------------------------
// ENVOI EMAIL SMTP (n destinataires)
// ---------------------------------------------------------
void sendEmail(String subject, String message) {
  WiFiClientSecure client;
  client.setInsecure();

  Serial.println("Connexion au serveur SMTP...");

  if (!client.connect(smtp_server, smtp_port)) {
    Serial.println("Erreur connexion SMTP !");
    return;
  }

  String login64 = encodeBase64(email_login);
  String pass64 = encodeBase64(email_password);

  client.println("EHLO esp32");
  delay(200);
  client.println("AUTH LOGIN");
  delay(200);
  client.println(login64);
  delay(200);
  client.println(pass64);
  delay(200);

  client.println("MAIL FROM:<" + String(email_from) + ">");
  delay(200);

  // RCPT TO pour tous les destinataires
  for (int i = 0; i < nbRecipients; i++) {
    client.println("RCPT TO:<" + String(recipients[i]) + ">");
    delay(100);
  }

  client.println("DATA");
  delay(200);

  // Header du mail
  client.println("Subject: " + subject);
  client.print("From: <" + String(email_from) + ">\nTo: ");
  for (int i = 0; i < nbRecipients; i++) {
    client.print("<" + String(recipients[i]) + ">");
    if (i < nbRecipients - 1) client.print(", ");
  }
  client.println();
  client.println("Content-Type: text/plain");
  client.println();
  client.println(message);
  client.println(".");
  client.println("QUIT");

  Serial.println("Email envoyé !");
}

// ---------------------------------------------------------
// SETUP
// ---------------------------------------------------------
void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, wifi_password);
  Serial.print("Connexion WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnecté !");
  Serial.print("IP : ");
  Serial.println(WiFi.localIP());

  for (int i = 0; i < 4; i++) {
    pinMode(switches[i], INPUT);
  }

  sendEmail("Reboot ESP32 alarme", "L'ESP32 de l'alarme vient de redémarrer. IP: " + WiFi.localIP().toString());
}

// ---------------------------------------------------------
// LOOP
// ---------------------------------------------------------
void loop() {
  int s0 = digitalRead(switches[0]);
  int s1 = digitalRead(switches[1]);
  int s2 = digitalRead(switches[2]);
  int s3 = digitalRead(switches[3]);

  String msg;

  if (s0 == 1) msg = "ALARM!";
  else if (s1 == 0) msg = "PANIC!";
  else if (s2 == 0) msg = "Alarm set";
  else if (s3 == 0) msg = "Awaiting restore";
  else msg = "Alarm unset";

  if (msg != lastMsg) {
    Serial.println("Changement détecté: " + msg);
    sendEmail("ALARME DE LA MAISON", msg);
    lastMsg = msg;
  }

  delay(1);
}
