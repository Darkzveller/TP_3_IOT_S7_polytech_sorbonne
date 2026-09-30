#include <Arduino.h>
#include <WiFi.h>

bool envoyerCommande(String commande, unsigned long timeout = 2000)
{
    // Vide les anciennes donnees dans le buffer UART
    while (Serial2.available())
    {
        Serial2.read();
    }

    Serial.print("Commande envoyee : ");
    Serial.println(commande);

    Serial2.println(commande);

    String reponse = "";
    unsigned long debut = millis();

    // Attend la reponse du LoRa-E5
    while (millis() - debut < timeout)
    {
        while (Serial2.available())
        {
            char c = Serial2.read();
            reponse += c;

            // Affiche directement ce que repond le LoRa-E5
            Serial.write(c);
        }
    }

    Serial.println();

    // Aucune reponse
    if (reponse.length() == 0)
    {
        Serial.println("ERREUR : aucune reponse du LoRa-E5");
        return false;
    }

    // Detection d'une erreur dans la reponse
    if (reponse.indexOf("ERROR") != -1 ||
        reponse.indexOf("FAIL") != -1)
    {
        Serial.println("ERREUR : commande refusee");
        return false;
    }

    Serial.println("Commande OK");
    Serial.println();

    return true;
}

void setup_LoraE5(void)
{
    Serial2.begin(9600);

    delay(100);

    Serial.println();
    Serial.println("===== Initialisation LoRa-E5 =====");

    if (!envoyerCommande("AT"))
    {
        Serial.println("LoRa-E5 non detecte !");
        return;
    }

    Serial.println("LoRa-E5 detecte !");
    Serial.println();

    envoyerCommande("AT+ID=AppEUI,0000000000000000");
    envoyerCommande("AT+ID=DevEUI,70B3D57ED007922E");
    envoyerCommande("AT+KEY=APPKEY,AF083CDC5BE612AAEB359989124B096B");

    envoyerCommande("AT+MODE=LWOTAA");
    envoyerCommande("AT+DR=DR3");

    envoyerCommande("AT+JOIN");
}
bool estEduroam(const String &ssid)
{
    return ssid == "eduroam";
}
struct PointAcces
{
    int salle;
    const char *bssid;
    int rssiReference;
    int channel;
};

PointAcces pointsAcces[] =
    {
        {203, "58:97:BD:D0:96:62", -77, 1},
        {308, "58:97:BD:CD:9C:62", -72, 11},
        {319, "58:97:BD:CD:93:E2", -66, 6},
        {107, "58:97:BD:07:D6:64", -50, 6}};

const int nombrePointsAcces =
    sizeof(pointsAcces) / sizeof(pointsAcces[0]);
int trouverSalle(String bssid)
{
    for (int i = 0; i < nombrePointsAcces; i++)
    {
        if (bssid.equalsIgnoreCase(pointsAcces[i].bssid))
        {
            return pointsAcces[i].salle;
        }
    }

    return -1;
}

void setup()
{
    Serial.begin(9600);
    delay(2000);

    Serial.println();
    Serial.println("BW16 + PlatformIO ready");

    // Initialise le driver Wi-Fi
    Serial.println("Initializing WiFi...");
    int status = WiFi.status();

    Serial.print("WiFi status: ");
    Serial.println(status);

    delay(2000);

    // Vérification rapide avec l'adresse MAC
    byte mac[6];
    WiFi.macAddress(mac);

    Serial.print("MAC: ");
    for (int i = 0; i < 6; ++i)
    {
        if (i > 0)
        {
            Serial.print(":");
        }

        if (mac[i] < 0x10)
        {
            Serial.print("0");
        }

        Serial.print(mac[i], HEX);
    }
    Serial.println();

    Serial.println();
    Serial.println("Scanning 2.4 GHz Wi-Fi networks...");

    const int count = WiFi.scanNetworks();

    Serial.print("scanNetworks() returned: ");
    Serial.println(count);

    if (count < 0)
    {
        Serial.println("Wi-Fi scan failed.");
        return;
    }

    if (count == 0)
    {
        Serial.println("No network found.");
        return;
    }

    Serial.print(count);
    Serial.println(" network(s) found:");
    /*
        for (int index = 0; index < count; ++index)
        {
            Serial.print(index + 1);
            Serial.print(". ");

            Serial.print(WiFi.SSID(index));
            Serial.print(" | RSSI: ");
            Serial.print(WiFi.RSSI(index));
            Serial.print(" dBm");
            Serial.print(" | Channel: ");
            Serial.print(WiFi.channel(index));
            Serial.print(" | BSSID: ");
            Serial.println(WiFi.BSSIDstr(index));
        }*/
    /*
 for (int index = 0; index < count; ++index)
 {
     String ssid = WiFi.SSID(index);

     if (estEduroam(ssid))
     {
         Serial.print("SSID: ");
         Serial.print(ssid);

         Serial.print(" | RSSI: ");
         Serial.print(WiFi.RSSI(index));
         Serial.print(" dBm");

         Serial.print(" | Channel: ");
         Serial.print(WiFi.channel(index));

         Serial.print(" | BSSID: ");
         Serial.println(WiFi.BSSIDstr(index));
     }
 }
*/
    for (int index = 0; index < count; ++index)
    {
        if (WiFi.SSID(index) == "eduroam")
        {
            String bssid = WiFi.BSSIDstr(index);
            int rssi = WiFi.RSSI(index);

            Serial.print("eduroam");
            Serial.print(" | RSSI: ");
            Serial.print(rssi);
            Serial.print(" dBm");

            Serial.print(" | Channel: ");
            Serial.print(WiFi.channel(index));

            Serial.print(" | BSSID: ");
            Serial.print(bssid);

            int salle = trouverSalle(bssid);

            if (salle != -1)
            {
                Serial.print(" | SALLE TROUVEE : ");
                Serial.print(salle);
            }
            else
            {
                Serial.print(" | Salle inconnue");
            }

            Serial.println();
        }
    }
   
 setup_LoraE5();
}

void loop()
{
    delay(1000);
}