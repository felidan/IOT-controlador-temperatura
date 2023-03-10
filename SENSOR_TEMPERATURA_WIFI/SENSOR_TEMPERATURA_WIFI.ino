#include <ESP8266WiFi.h>
#include "DHT.h"

#define DHTPIN 2 // PINO DE DADOS
#define TRANSISTORPIN 14 // PINO DO TRANSISTOR
#define LIMITETEMPERATURA 23.5 // LIMITE DA TEMPERATURA
#define DHTTYPE DHT11 // MODELO DO SENSOR

DHT dht(DHTPIN, DHTTYPE); // INIT

const char* ssid = "Felipe";
const char* senha = "Kf040921@";
char* status_ventilador = "";

WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  dht.begin();
  delay(10);

  pinMode(TRANSISTORPIN, OUTPUT);
  
  Serial.print("Conectando a rede:");
  Serial.println(ssid);
  WiFi.begin(ssid, senha);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi conectado");

  server.begin();
  Serial.println("Servidor iniciado");

  Serial.print("Use esta URL: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  status_ventilador = "LIGADO";
  digitalWrite(TRANSISTORPIN, LOW);
}

void loop() {

  float u = dht.readHumidity();
  float t = dht.readTemperature();
  
  if(isnan(t) || isnan(u)){
    Serial.println("Falha de leitura");
    }
  else{
      Serial.print("Umidade: ");
      Serial.println(u);
      Serial.print("Temperatura: ");
      Serial.println(t);


      if(t > LIMITETEMPERATURA){
        Serial.print("Temperatura acima do limite de ");
        Serial.println(LIMITETEMPERATURA);
        status_ventilador = "LIGADO";
        digitalWrite(TRANSISTORPIN, LOW);
      }
      else if(t <= (LIMITETEMPERATURA -0.5)){
        Serial.print("Temperatura abaixo do limite de ");
        Serial.println(LIMITETEMPERATURA);
        status_ventilador = "DESLIGADO";
        digitalWrite(TRANSISTORPIN, HIGH);
      }

      
  }
  clientServer(u, t);
  
  delay(5000);
}

void clientServer(float u, float t)
{
  WiFiClient cliente = server.available();
      
      if(!cliente){
        Serial.println("Sem requisições do servier para processar..");
      }
      else{
        Serial.println("Novo cliente");
        while(!cliente.available()){
          delay(500);
        }

        String requisicao = cliente.readStringUntil('\r');
        Serial.println(requisicao);
        cliente.flush();
        
        getClient(u, t, cliente);
     }
}
      
void getClient(float u, float t, WiFiClient cliente)
{
  cliente.println("HTTP/1.1 200 OK");
  cliente.println("Content-Type:text/html");
  cliente.println("");
  cliente.println("<!DOCTYPE HTML>");
  cliente.println("<html>");
  cliente.println("<head>");
  cliente.println("    <title>Temperatura das Chinchilas IOT</title>");
  cliente.println("    <meta charset=\"utf-8\">");
  cliente.println("    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  cliente.println("    <link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.4.1/css/bootstrap.min.css\">");
  cliente.println("    <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.5.1/jquery.min.js\"></script>");
  cliente.println("    <script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.4.1/js/bootstrap.min.js\"></script>");
  cliente.println("</head>");
  cliente.println("<script type=\"text/javascript\">");
  cliente.println("  window.onload = function () {");
  cliente.println("  now = new Date;  ");
  cliente.println("document");
  cliente.println("    .getElementById('ultimaAtualização')");
  cliente.println("      .innerHTML = \"Última atualização: \" + now.getDate() + \"/\" + now.getMonth() + \"/\" + now.getFullYear() + \", às \" + now.getHours() + \":\" + now.getMinutes() + \":\" + now.getSeconds() + \".\"");
  cliente.println("  }");
  cliente.println("</script>");
  cliente.println("<body>");
  cliente.println("    <nav class=\"navbar navbar-inverse\">");
  cliente.println("        <div class=\"container-fluid\">");
  cliente.println("            <div class=\"navbar-header\">");
  cliente.println("                <a class=\"navbar-brand\" href=\"#\">Controle de Temperatura das Chinchilas</a>");
  cliente.println("            </div>");
  cliente.println("        </div>");
  cliente.println("    </nav>");
  cliente.println("    <div class=\"container\">");
  cliente.println("        <div class=\"row\">");
  cliente.println("            <div class=\"col-sm-6 col-xs-6\">");
  cliente.println("                <div class=\"form-group\">");
  cliente.println("                    <label for=\"umidade\">");
  cliente.println("                        Umidade Atual: <i>");
  // UMIDADE
  cliente.print(u);
  cliente.println("                        </i>");
  cliente.println("                    </label>");
  cliente.println("                </div>");
  cliente.println("            </div><div class=\"col-sm-6 col-xs-6\">");
  cliente.println("                <label>");
  cliente.println("                    <button type=\"button\" class=\"btn btn-danger\" disabled>-</button>");
  cliente.println("                    <label>");
  cliente.println("                        -");
  cliente.println("                    </label>");
  cliente.println("                    <button type=\"button\" class=\"btn btn-success\" disabled>+</button>");
  cliente.println("                </label>");
  cliente.println("            </div>");
  cliente.println("        </div><hr><div class=\"row\">");
  cliente.println("            <div class=\"col-sm-6 col-xs-6\">");
  cliente.println("                <div class=\"form-group\">");
  cliente.println("                    <label for=\"temperatura\">");
  cliente.println("                        Temperatura Atual: <i>");
  // TEMPERATURA
  cliente.print(t);
  cliente.println("                        </i>");
  cliente.println("                    </label>");
  cliente.println("                </div>");
  cliente.println("            </div><div class=\"col-sm-6 col-xs-6\">");
  cliente.println("                <label>");
  cliente.println("                    <button type=\"button\" class=\"btn btn-danger\" disabled>-</button>");
  cliente.println("                    <label>");
  // LIMITE DE TEMPERATURA
  cliente.print(LIMITETEMPERATURA);
  cliente.println("                    </label>");
  cliente.println("                    <button type=\"button\" class=\"btn btn-success\" disabled>+</button>");
  cliente.println("                </label>");
  cliente.println("            </div>");
  cliente.println("        </div>");
  cliente.println("        <hr><div class=\"row\">");
  cliente.println("            <div class=\"col-sm-6 col-xs-6\">");
  cliente.println("                <div class=\"form-group\">");
  cliente.println("                    <label>");
  cliente.println("                        <strong>");
  cliente.println("                            Status do ventilador:");
  // STATUS DO VENTILADOR
  cliente.print(status_ventilador);
  cliente.println("                        </strong>");
  cliente.println("                    </label>");
  cliente.println("                </div>");
  cliente.println("            </div>");
  cliente.println("        </div>");
  cliente.println("        <hr>");
  cliente.println("        <p id=\"ultimaAtualização\"></p>");
  cliente.println("    </div>");
  cliente.println("</body>");
  cliente.println("</html>");
}
