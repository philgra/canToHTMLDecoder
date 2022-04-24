//HTML Code für die Website Addierung der Strings, da Arduino keinen String über mehrere Zeilen zulässt -> Lesbarkeit...

String page = "<h1>WLAN Can-Decoder V1.0</h1><h1>Incoming data:</h1> <h1 id=\"data\">""</h1>\r\n<script>\r\n"
              + (String) "var x = setInterval(function() {loadData(\"data.txt\",updateData)}, "
              + String(1000)    //Update Zeit in ms
              + (String) ");\r\n"
              + (String) "function loadData(url, callback){\r\n"
              + (String) "var xhttp = new XMLHttpRequest();\r\n"
              + (String) "xhttp.onreadystatechange = function(){\r\n"
              + (String) " if(this.readyState == 4 && this.status == 200){\r\n"
              + (String) " callback.apply(xhttp);\r\n"
              + (String) " }\r\n"
              + (String) "};\r\n"
              + (String) "xhttp.open(\"GET\", url, true);\r\n"
              + (String) "xhttp.send();\r\n"
              + (String) "}\r\n"
              + (String) "function updateData(){\r\n"
              + (String) " document.getElementById(\"data\").innerHTML = this.responseText;\r\n"
              + (String) "}\r\n"
              + (String) "</script>\r\n"
              + (String) "<body>"
              + (String) "<button onclick=\"window.location.href = '/reset';\">Reset</button>"
              + (String) "</body>";
