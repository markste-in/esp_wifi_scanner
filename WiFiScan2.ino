/*
 *  This sketch demonstrates how to scan WiFi networks.
 *  The API is almost the same as with the WiFi Shield library,
 *  the most obvious difference being the different file you need to include:
 */
#include "WiFi.h"
#include <vector>

typedef struct
{
  String networkname;
  int seen;
  String bssid;
  byte encryptiontype;
  unsigned long lastseen;
  long rssi;
} Network;
std::vector<Network> nets;

bool cmp_timestmp(const Network& x, const Network& y) 
{ 
  return x.lastseen > y.lastseen; 
}

void setup()
{
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    Serial.println("Setup done");
}

void loop()
{
    unsigned long now = millis();
    Serial.println(now);
    Serial.println("scan start");

    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } 
    else 
    {
        Serial.print(n);
        Serial.println(" networks found");
        Serial.print(nets.size());
        Serial.println(" known networks");
        for (int i = 0; i < n; ++i) 
        {
            bool in_db = false;
            //Check if network is already known and update "seen" if true
            for(std::vector<Network>::iterator it = nets.begin(); it != nets.end(); ++it) {
                if ((*it).bssid== WiFi.BSSIDstr(i))
                {
                in_db = true;
                //Serial.print(" Seen: ");
                (*it).seen++;
                (*it).lastseen=millis();
                (*it).rssi = (*it).rssi + (WiFi.RSSI(i)-(*it).rssi)/((*it).seen); // Incremental mean of the RSSI
                break;
                }
             }

            //if item is not in db, add it
            if (!in_db){
              nets.push_back({
                WiFi.SSID(i).c_str(),
                1,
                WiFi.BSSIDstr(i).c_str(),
                WiFi.encryptionType(i),
                millis(),
                WiFi.RSSI(i)
                });                
            }             
        }       
        std::sort(nets.begin(), nets.end(), cmp_timestmp);
        Serial.println("SSID\t\t      | BSSID\t     | xSeen | Last Seen |  Enc  | Mean RSSI");
        for(std::vector<Network>::iterator it = nets.begin(); it != nets.end(); ++it) {   
          char prBuffer[255] ={"\0"}; 
          sprintf(prBuffer, "%-25s | \0",it->networkname.substring(0, 25).c_str());
          Serial.print(prBuffer);           
          Serial.print(it->bssid + " | ");
          sprintf(prBuffer, "%5d | \0",it->seen);
          Serial.print(prBuffer);
          sprintf(prBuffer, "%9.1f | \0",(millis() - it->lastseen) / 1000.0 );
          Serial.print(prBuffer);
          sprintf(prBuffer, "%5d | \0",it->encryptiontype);
          Serial.print(prBuffer);
          Serial.println(it->rssi);
        }  
    }
    Serial.println("");
    delay(100);
}
