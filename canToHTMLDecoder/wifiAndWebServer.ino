
/*
   Diese Funktion stellt die Verbindung mit dem WLAN her und initialisiert den Webserver.
*/
void setupNetwork()
{
#ifdef GEN_AP
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  //Api Mode
  WiFi.mode(WIFI_AP);

  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP("CAN Decoder", NULL);
  /* Setup the DNS server redirecting all the domains to the apIP */
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", apIP);
#else
  WiFi.begin(c_g_ssid, c_g_wlanPassword); //begin WiFi connection
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(c_g_ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

#endif
  server.on("/data.txt", []() {
    server.send(200, "text/html", g_html_data);
  });

  server.onNotFound([]() {
    server.send(200, "text/html", page);
  });


  server.on("/", []() {
    server.send(200, "text/html", page);
  });

  // Receive an reset request
  server.on("/reset", []() {
    g_resetRequested = true;
    server.send(200, "text/html", "<body>Reset...</body><meta http-equiv=\"refresh\" content=\"1; URL=/\">");
  });

  server.begin();
  Serial.println("Web server started!");
}
