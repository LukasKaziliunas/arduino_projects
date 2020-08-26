
#include <Arduino_JSON.h>
#include <SPI.h>
#include <Ethernet.h>

class HttpRequestParser {

private:
  String method = "";
  String uri= "";
  JSONVar body;

public:
  HttpRequestParser(String firstLine, String body);
  String getMethod();
  String getUri();
  JSONVar getBody();
};

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 15);

EthernetServer server(85);
String firstLine; //first http request header line
String bodyString; //http request body goes in here
int lamp = 8;
int vent = 7;
int pump = 6;

short lampStat = 0;
short ventStat = 0;
short pumpStat = 0;

void setup() {
  Serial.begin(9600);
  pinMode(lamp , OUTPUT);
  pinMode(vent , OUTPUT);
  pinMode(pump , OUTPUT);
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("Server is at ");
  Serial.println(Ethernet.localIP());
}
void loop()
{
 EthernetClient client = server.available();
 if (client) {
   Serial.println("Client request");
   // jei linija tuscia tai http uklausa baigese
   boolean currentLineIsBlank = true;
   int line = 1;
   while (client.connected()) {
     while(client.available()) {
       char c = client.read();
       if(line == 1)
       {
        firstLine += c;
       }
       Serial.print(c);

       // headeriai ir body atskirti tuscia linija, jei yra \n simbilis ir nebuvo rasta jokio
       //simpolio toje linijoje, reiskia kitoje eiluteje prasideda Body
       if (c == '\n' && currentLineIsBlank) {

// body
         while(client.available())
         {
            bodyString += (char)client.read();
         }

         Serial.println("Sending response");

         client.println("HTTP/1.0 200 OK");
         client.println("Access-Control-Allow-Origin: *");

         HttpRequestParser httpRP = HttpRequestParser(firstLine, bodyString);
        
         String resp = handleRequest(httpRP);
         if(resp != "")
         {
          client.println("Content-Type: application/json");
          client.println();
          client.println(resp);
         }else
         {
          client.println("Content-Type: text/plain");
          client.println();
         }
          delay(2);
         client.stop();
         

         // debug
         Serial.println("body: ");
         Serial.println(JSON.stringify(httpRP.getBody()));
         Serial.println("Method: ");
         Serial.println(httpRP.getMethod());
         Serial.println("Uri: ");
         Serial.println(httpRP.getUri());       
         
         bodyString = "";
         firstLine = "";
       }
       else if (c == '\n') {
         // you're starting a new line
         line++;
         currentLineIsBlank = true;
       }
       else if (c != '\r') {
         // you've gotten a character on the current line
         currentLineIsBlank = false;
       }
     }
   }
   Serial.println("Disconnected");
 }
}

HttpRequestParser::HttpRequestParser(String firstLine, String body)
{
    String method_ = firstLine.substring(0, firstLine.indexOf(" "));
    String temp = firstLine.substring(firstLine.indexOf(" ")+1);
    String uri_ = temp.substring(0, temp.indexOf(" "));
    this->method = method_;
    this->uri = uri_;

    JSONVar jsonObj = JSON.parse(body);
    this->body = jsonObj;
}

String HttpRequestParser::getMethod()
{
    return this->method;
}
String HttpRequestParser::getUri()
{
    return this->uri;
}
JSONVar HttpRequestParser::getBody()
{
    return this->body;
}

String handleRequest(HttpRequestParser h)
{
  if(h.getMethod() == "POST")
  {
    handlePOSTrequest(h);
    return "";
  }else if(h.getMethod() == "GET")
  {
    return handleGETrequest(h);
  }
}

void handlePOSTrequest(HttpRequestParser h)
{
  JSONVar json = h.getBody();
    if(json.hasOwnProperty("lamp")) {
    digitalWrite(lamp, (int)json["lamp"]);
    lampStat = (int)json["lamp"];
    }
    else if(json.hasOwnProperty("vent")) {
    digitalWrite(vent, (int)json["vent"]);
    ventStat = (int)json["vent"];
    }
    else if(json.hasOwnProperty("pump")) {
    digitalWrite(pump, (int)json["pump"]);
    pumpStat = (int)json["pump"];
    }
}

String handleGETrequest(HttpRequestParser h)
{
  String query = h.getUri();
   if(query.indexOf("favicon.ico") > 0)
    {
      return "";
    }
    else if(query.indexOf("?temp") > 0)
    {
      JSONVar response;
      response["temp"] = 25;
      return JSON.stringify(response);
    }
    else if(query.indexOf("?moist") > 0)
    {
      JSONVar response;
      response["moist"] = 555;
      return JSON.stringify(response);
    }
    else if(query.indexOf("?lamp") > 0)
    {
      JSONVar response;
      response["state"] = lampStat;
      return JSON.stringify(response);
    }
    else if(query.indexOf("?vent") > 0)
    {
      JSONVar response;
      response["state"] = ventStat;
      return JSON.stringify(response);
    }
    else if(query.indexOf("?pump") > 0)
    {
      JSONVar response;
      response["state"] = pumpStat;
      return JSON.stringify(response);
    }else
    {
      return "";
    }
}
