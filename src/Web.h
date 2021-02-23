// Constants
// const char *ssid = "NETGEAR80";
// const char *password = "thelmarocks";
const char *ssid = "MONKEY";
const char *password = "teddybear";
const int dns_port = 53;
const int http_port = 80;
const int ws_port = 1337;

AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(1337);

/***********************************************************
   Functions
*/

// Callback: receiving any WebSocket message
void onWebSocketEvent(uint8_t client_num,
                      WStype_t type,
                      uint8_t *payload,
                      size_t length)
{

  // Figure out the type of WebSocket event
  switch (type)
  {

  // Client has disconnected
  case WStype_DISCONNECTED:
    Serial.printf("[%u] Disconnected!\n", client_num);
    break;

  // New client has connected
  case WStype_CONNECTED:
  {
    IPAddress ip = webSocket.remoteIP(client_num);
    Serial.printf("[%u] Connection from ", client_num);
    Serial.println(ip.toString());
  }
  break;

  // Handle text messages from client
  case WStype_TEXT:

    // Print out raw message
    Serial.printf("[%u] Received text: %s\n", client_num, payload);

    if (strncmp((char *)payload, "data", 4) == 0)
    {
      File DBFile = SPIFFS.open("/database.json", FILE_WRITE);
      if (!DBFile)
      {
        Serial.println("There was an error opening the file for writing");
        return;
      }
      if (DBFile.print((char *)payload + 6))
      {
        Serial.println("File was written");
        ;
      }
      else
      {
        Serial.println("File write failed");
      }
      DBFile.close();
    }
    else if (strncmp((char *)payload, "prst", 4) == 0)
    {
      Serial.println("Recieved Payload");
      JSONtoPreset((char *)payload + 6);
    }
    else if (strncmp((char *)payload, "palt", 4) == 0)
    {
      Serial.println("Recieved Palette");
      JSONtoPalette((char *)payload + 6);
    }
    else if (strncmp((char *)payload, "dlta", 4) == 0)
    {
      Serial.println("Recieved Delta");
      DB.delta = atof((char *)payload + 6);
    }
    break;
  // For everything else: do nothing
  case WStype_BIN:
  case WStype_ERROR:
  case WStype_FRAGMENT_TEXT_START:
  case WStype_FRAGMENT_BIN_START:
  case WStype_FRAGMENT:
  case WStype_FRAGMENT_FIN:
  default:
    break;
  }
}

// Callback: send homepage
void onIndexRequest(AsyncWebServerRequest *request)
{
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                 "] HTTP GET request of " + request->url());
  request->send(SPIFFS, "/index.html", "text/html");
}

void onDataRequest(AsyncWebServerRequest *request)
{
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                 "] HTTP GET request of " + request->url());
  request->send(SPIFFS, "/database.json", "text/html");
}

// Callback: send style sheet
void onCSSRequest(AsyncWebServerRequest *request)
{
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                 "] HTTP GET request of " + request->url());
  request->send(SPIFFS, "/style.css", "text/css");
}

// Callback: send 404 if requested file does not exist
void onPageNotFound(AsyncWebServerRequest *request)
{
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                 "] HTTP GET request of " + request->url());
  request->send(404, "text/plain", "Not found");
}

void webInit()
{

  // Make sure we can read the file system
  if (!SPIFFS.begin())
  {
    Serial.println("Error mounting SPIFFS");
    while (1)
      ;
  }

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // On HTTP request for root, provide index.html file
  server.on("/", HTTP_GET, onIndexRequest);

  // On HTTP request for style sheet, provide style.css
  server.on("/style.css", HTTP_GET, onCSSRequest);

  // On HTTP request for database, provide index.html file
  server.on("/database.json", HTTP_GET, onDataRequest);

  // Handle requests for pages that do not exist
  server.onNotFound(onPageNotFound);

  // Start web server
  server.begin();

  // Start WebSocket server and assign callback
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
}

void webLoop()
{
  webSocket.loop();
}
