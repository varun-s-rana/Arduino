#include <SPI.h>
#include <Ethernet.h> // Initialize the libraries.

byte mac[] = { 0x90, 0xA2, 0xDA, 0x0E, 0x93, 0xF7 }; // MAC address of Ethernet Shield
byte ip[] = { 192,168,0,107 }; // IP Address
EthernetServer server(80); // Assigning the port forwarded number
String readString; // We will be using strings to keep track of things.
int val1=0; // Initialize Speed of bridge
int val2=0; // Initialize bridge start/stop value
int val3=0; // initialize bridge opening sequence/ closing sequence value
int pot_old1=500;

void setup()
{
    Serial.begin(9600);
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(2, OUTPUT);
    Ethernet.begin(mac, ip);
}
void loop()
{
    int pot= analogRead(0); // Bridge limit switch value
    byte MSB= highByte(pot);
    byte LSB=lowByte(pot);

    Serial.write(MSB); //Sends potentiometer values to the serial port
    Serial.write(LSB);

    if( (pot>925) && (pot_old1<925) && val3==HIGH ) //closing threshold
    {
        val2=LOW;
    }

    if( (pot_old1<530) && (pot>530) && val3==LOW) //opening threshold
    {
        val2=LOW;
    }
    pot_old1=pot-10;
    // The following code drives the DC motor using pulse width modulation
    if(val2==HIGH)
    {
        if(val3==HIGH)
        {
            digitalWrite(6, HIGH); // INPUT 4
            digitalWrite(5, LOW); // INPUT 3
            delay(val1/100+1);
            digitalWrite(6, LOW);
            digitalWrite(5, LOW);
            delay(11-val1/100);
        }

        if(val3==LOW)
        {
            digitalWrite(6, LOW); // INPUT 4
            digitalWrite(5, HIGH); // INPUT 3

            delay(val1/100+1); // wait for a second

            digitalWrite(6, LOW);
            digitalWrite(5, LOW);

            delay(11-val1/100); // wait for a second
        }
    }

    // The following code sets up the web server on the ethernet shield

    EthernetClient client = server.available();
    if (client) 
    {
        while (client.connected()) 
        {
            if (client.available()) 
            {
                char c = client.read();
                if (readString.length() < 100) 
                {
                    readString += c;
                }
                if (c == '\n') 
                {
                    Serial.println(readString); // And here we begin including the HTML
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println();
                    client.println("<hmtl>");
                    client.println("<head>");
                    client.println("ARDUINO PAGE");
                    client.println("</head>");
                    client.println("<title>");
                    client.println("ARDUINO + ETHERNET Page");
                    client.println("</title>");
                    client.println("<body bgcolor=blue>");
                    client.println("<font color=white>");
                    client.println("<meta http-equiv=\"refresh\" content=\"300\">"); // This is used to refresh the page so
                    client.println("<center>");
                    client.println("<b>");
                    client.println("Greetings! The following web page is hosted on the arduino web server and can be ");
                    client.println("</br>");
                    client.println("used to control the opening and closing sequence of a Strauss Bascule bridge");
                    client.println("</br>");
                    client.println("Enjoy!");
                    client.println("</b>");
                    client.println("<p>");
                    client.println("<FORM>");
                    client.println("<INPUT type=button value=BRIDGE-ON onClick=window.location='/?bridgeon1\'>");
                    client.println("<INPUT type=button value=BRIDGE-OFF onClick=window.location='/?bridgeoff1\'>");
                    client.println("</FORM>"); // Above and below you'll see that when we click on a button, it adds a little
                    snippet
                    client.println("<FORM>"); // to the end of the URL. The Arduino watches for this and acts accordingly.
                    client.println("<INPUT type=button value=OPENING_MODE onClick=window.location='/?open\'>");
                    client.println("<INPUT type=button value=CLOSING_MODE onClick=window.location='/?close\'>");
                    client.println("<INPUT type=button value=LOW_SPEED onClick=window.location='/?low_speed\'>");
                    client.println("<INPUT type=button value=MEDIUM_SPEED onClick=window.location='/?medium_speed\'>");
                    client.println("<INPUT type=button value=HIGH_SPEED onClick=window.location='/?high_speed\'>");
                    client.println("</FORM>");
                    client.println("</center>");
                    client.println("</font>");
                    client.println("</body>");
                    client.println("</html>");
                    delay(1);

                    if(readString.indexOf("?bridgeon") >0) // Arduino is parsing the code to look for snippets
                    {
                        val2=HIGH;
                    }
                    else
                    {
                        if(readString.indexOf("?bridgeoff") >0)
                        {
                            val2=LOW;
                        }

                        else
                        {
                            if(readString.indexOf("?open") >0)// opening
                            {              
                                val3=LOW;
                            }

                            else
                            {
                                if(readString.indexOf("?close") >0)// closing
                                {
                                    val3=HIGH;
                                }

                                else
                                {
                                    if(readString.indexOf("?low_speed") >0)
                                    {
                                        val1=300;
                                    }
                                    else
                                    {
                                        if(readString.indexOf("?medium_speed") >0)
                                        {
                                            val1=600;
                                        }
                                        else
                                        {
                                            if(readString.indexOf("?high_speed") >0)
                                            {
                                                val1=750;
                                            }

                                        }
                                    }
                                }
                            }
                        }
                    }
                    readString="";
                    client.stop(); // End of session.
                }
            }
        }
    }
}