/*
   Diese Funktion wandelt die Messwerte des Fahrzeugs in einen String um.
   Dieser ist für die HTML Ausgabe gedacht.
*/
String carDataToHTMLString(carData input)
{
  String buff = "<br/><span style=\"font-size:20px\">";
  if (input.coolantTempRec)
  {
    buff += "Coolant temperature = " + String(input.coolantTemp) + " °C <br />";
  }
  if (input.engineRpmRec)
  {
    buff += "Engine RPM = " + String(input.engineRpm) + "<br />";
  }
  if (input.velocityRec)
  {
    buff += "Velocity = " + String(input.velocity) + " km/h<br />";
  }
  if (input.voltageRec)
  {
    buff += "Voltage = " + String(input.voltage) + " V<br />";
  }
  buff += "</span>";

  return buff;
}


/*
   Diese Funktion wandelt eine CanMessage (input") in einen String um.
   Dieser ist für die HTML Ausgabe als Tabelle gedacht.
   Beispiel:
   "<tr><td>myID1</td><td>myDLC1</td><td>myData1</td><td>111</td><td>9988</td></tr>"
*/
String singleCanMessageToHTMLString(canMessage input)
{
  String outp =
    "<tr><td>" +
    input.id + "</td><td>" +
    input.dlc + "</td><td>" +
    input.data + "</td><td>" +
    String(input.period) + "</td><td>" +
    String(input.count) + "</td>" + "</tr>";
  return outp;
}




/*
   Diese Funktion wandelt eine Array von CanMessages ("input[]") mit "length" Elementen in einen String um.
   Dieser ist für die HTML Ausgabe als Tabelle gedacht.
   Beispiel:
   "
   <table border="2" cellspacing="2" cellpadding="6">
   <tbody>
    <tr>
      <th>ID</th>
      <th>DLC</th>
      <th>Data</th>
      <th>Period / [ms]</th>
      <th>Count</th>
      </tr>
    <tr>
      <td>myID0</td>
      <td>myDLC0</td>
      <td>myData0</td>
      <td>111</td>
      <td>9988</td>
    </tr>
    ...
    <tr>
      <td>myID10</td>
      <td>myDLC10</td>
      <td>myData10</td>
      <td>111</td>
      <td>9987</td>
    </tr>
   </tbody>
   </table>
   "
*/
String canMessagesToHTMLString(canMessage input[], int length)
{
  String buff = "<table  border=\"2\" cellspacing=\"2\" cellpadding=\"6\"><tr><th>ID</th><th>DLC</th><th>Data</th><th>Period / [ms]</th><th>Count</th></tr>";
  for (int i = 0; i < length; i++)
  {
    buff += singleCanMessageToHTMLString(input[i]);
    yield();
  }
  buff += "</table>";
  return buff;
}


/*
   Diese Funktion wandelt eine CanMessage (input") in einen String um.
   Dieser ist für die serielle Ausgabe gedacht.
   Beispiel:
   "| myID0      | myDLC0   | myData0          |      113 |     8320 |"
*/
String canToString(canMessage input)
{
  String outp =
    "| " +
    strToConstLength(input.id,  10, false) + " | " +
    strToConstLength(input.dlc, 8, false) + " | " +
    strToConstLength(input.data, 16, false) + " | " +
    strToConstLength(String(input.period), 8, true) + " | " +
    strToConstLength(String(input.count), 8, true) + " |";
  return outp;
}

/*
   Diese Funktion wandelt eine Array von CanMessage s ("input[]") mit "length" Elementen in einen String um.
   Dieser ist für die serielle Ausgabe gedacht.
   Beispiel:
   "
   | ID         | DLC      | DATA             |   PERIOD |    COUNT |
   | myID0      | myDLC0   | myData0          |      113 |     8320 |
   | myID1      | myDLC1   | myData1          |      113 |     8320 |
   | myID2      | myDLC2   | myData2          |      113 |     8320 |
   | myID3      | myDLC3   | myData3          |      112 |     8320 |
   | myID4      | myDLC4   | myData4          |      112 |     8320 |
   | myID5      | myDLC5   | myData5          |      118 |     8319 |
   | myID6      | myDLC6   | myData6          |      118 |     8319 |
   | myID7      | myDLC7   | myData7          |      119 |     8319 |
   | myID8      | myDLC8   | myData8          |      119 |     8319 |
   | myID9      | myDLC9   | myData9          |      119 |     8319 |
   | myID10     | myDLC10  | myData10         |      120 |     8319 |
   "
*/
String messagesToString(canMessage input[], int length)
{
  String buff =     "| " +
                    strToConstLength(String("ID"), 10, false) + " | " +
                    strToConstLength(String("DLC"), 8, false) + " | " +
                    strToConstLength(String("DATA"), 16, false) + " | " +
                    strToConstLength(String("PERIOD"), 8, true) + " | " +
                    strToConstLength(String("COUNT"), 8, true) + " |\n";
  for (int i = 0; i < length; i++)
  {
    buff += canToString(input[i]);
    buff += "\n";
    yield();
  }
  buff += "\n";
  return buff;
}

/*
   Diese Funktion gibt immer einen String mit der in "length" parametrierten länge zurück.
   Bei Text(isNumber = false) wird rechts vom "input" leerzeichen hinzugefügt, bei Zahlen ("isNumber" = true) links vom "input"
   Ist die Länge von"input" größer als "length" wird nur der linke Teil, mit der Länge = "length" des "input" verwendet.
*/
String strToConstLength(String input, int length, bool isNumber)
{
  String output = "";

  if (input.length() > length)
  {
    output = input.substring(0, length);
  }
  else if (input.length() < length)
  {
    if (!isNumber) output = input;

    for (int i = 0; i < (length - input.length()); i++)
    {
      output += " ";
    }

    if (isNumber) output += input;
  }
  else
  {
    output = input;
  }

  return output;
}
