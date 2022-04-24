

void initCan()
{ // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
  if (CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK)
    Serial.println("MCP2515 Initialized Successfully!");
  else
    Serial.println("Error Initializing MCP2515...");

  CAN0.setMode(MCP_NORMAL);                     // Set operation mode to normal so the MCP2515 sends acks to received data.

  pinMode(CAN0_INT, INPUT);                            // Configuring pin for /INT input
}

void sendRequestToCar(char number)
{

  byte dataToSend[8] = {0x02, 0x01, number, 0x00, 0x00, 0x00, 0x00, 0x00};
  // send data:  ID = 0x100, Standard CAN Frame, Data length = 8 bytes, 'data' = array of data bytes to send

  byte sndStat = CAN0.sendMsgBuf(0x7df, 0, 8, dataToSend);
  Serial.println("SndStat: " + String(sndStat == CAN_OK));

}

/*
 * Diese Funktion dient zum einfachen Setzen der Werte einer canMessage.
 * Als Argumente werden die ID, DLC und die Daten übergeben der Rückgabewert ist die erzeugte canMessage
 */
canMessage setCanMessage(String id,  String dlc, String data)
{
  canMessage message;
  message.id = id;
  message.dlc = dlc;
  message.data = data;
  return message;
}


/*
 * Diese Funktion dient zur Verarbeitung eingegangener CAN-Nachrichten
 * Es wird erst überprüft obe schon eine Nachricht von dieser ID empfangen wurde.
 * Ist dies der Fall werden DLC und Data auf die neuen Daten geupdatet.
 *  Dann wird die Periode berechnet (Zeit des letzten Empfangs bis jetzt in ms) und der Zähler dieser ID inkrementiert
 *  Auch die Zeit des letzten Empfangs wird geupdatet.
 *Wurde noch keine Nachricht dieser ID empfangen wird die ID neu angelegt und der Zähler für die Anzahl der empfangenen ID's inkrementiert
 *Um das Array mit den CanNAchrichten ändern zu können wird es per Referenz übergeben.
 *Der neue Wert mit der Anzahl der empfangenen ID's wird als int zruückgegeben.
 */
int canMessageReceived(canMessage storArray[], int cntr, canMessage input)
{
  String newID = input.id;
  bool idKnown = false;
  int mycntr = cntr;

  for (int i = 0; (i < cntr) && (!idKnown); i++)
  {
    if (storArray[i].id.equals(newID))
    {
      //Wenn gefunden -> updaten
      idKnown = true;
      storArray[i].dlc = input.dlc;
      storArray[i].data = input.data;
      storArray[i].period = millis() - storArray[i].lastReceivedMs ;
      storArray[i].count++;
      storArray[i].lastReceivedMs = millis();
    }
  }
  if (!idKnown)
  {
    //Neu anlegen
    mycntr++;
    if (MAX_NUMB_OF_IDS > mycntr)
    {
      storArray[cntr].id = input.id;
      storArray[cntr].dlc = input.dlc;
      storArray[cntr].data = input.data;
      storArray[cntr].period = 0;
      storArray[cntr].count = 1;
      storArray[cntr].lastReceivedMs = millis();
    }
    else
    {
      Serial.println("Maximale Anzahl der IDs erreicht!");
      mycntr--;
    }
  }

  return mycntr;
}
