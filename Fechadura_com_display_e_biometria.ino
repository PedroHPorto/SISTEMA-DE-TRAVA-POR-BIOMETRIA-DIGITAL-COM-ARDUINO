/*
 * Q0000
 * AUTOR:   Pedro Henrique
 * SKETCH:  Fechadura com Leitor Biométrico e Display
 * DATA:    08/08/2019
 */

// INCLUSÃO DAS BIBLIOTECAS
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <PushButton.h>
#include<LiquidCrystal.h>

const int pinoOut = 9; //PINO DIGITAL UTILIZADO PELO MÓDULO INFRAVERMELHO
  const int pinBio = 10;//PINO DIGITAL ATIVA O LEITOR BIOMÉTRICO


// DEFINIÇÃO DO PINO DO BOTÃO
#define pinBot 11

// DEFINIÇÃO DO PINO DA TRAVA
#define pinTrava 12

// INSTANCIANDO OBJETOS
SoftwareSerial mySerial(12, 13);//PINAGEM DO LEITOR BIOMETRICO

LiquidCrystal lcd(7,6, 5, 4, 3, 2); // Pinagem do LCD

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
PushButton botao(pinBot);

// DECLARAÇÃO DAS VARIÁVEIS E FUNCOES
uint8_t numID = 1;
bool gravar=false;

uint8_t modoGravacaoID(uint8_t IDgravar);


//Configuracoes LCD 16x2  
int screenWidth = 16;  
int screenHeight = 2;  
   
//String para as 2 linhas 
 
//line1 = Scroll linha superior
String line1 = "POSICIONE O DEDO NO LEITOR BIOMETRICO "; 
 
//line2 = Linha inferior - estatica  
String line2 = "P/LIBERAR ACESSO";  
   
// just some reference flags  
int stringStart, stringStop = 0;  
int scrollCursor = screenWidth;  
int tamanho =0;  
   

void setup() {

   pinMode(pinoOut, INPUT); //DEFINE O PINO COMO ENTRADA
  Serial.begin (9600); //INICIALIZA A SERIAL

  pinMode(pinTrava, OUTPUT);
  digitalWrite(pinTrava, HIGH);
  
  Serial.begin(9600);
  finger.begin(57600);

  if(finger.verifyPassword()){
    Serial.println("Sensor biometrico encontrado!");
  } else {
    Serial.println("Sensor biometrico não encontrado! Verifique a conexão e reinicie o sistema");
    while(true) { 
      delay(1); 
      }
  {  
  //Inicializacao do display  
  lcd.begin(screenWidth,screenHeight);  
}  
   
  }

  Serial.println("Fim do Setup!");
}

void loop() {
  botao.button_loop();

  if ( botao.pressed() ){
    gravar = true;
  }

  if(gravar){
    modoGravacaoID(0);
    gravar = false;
  }

  getFingerprintIDez();

  lcd.setCursor(scrollCursor, 1);  
  lcd.print(line1.substring(stringStart,stringStop));  
  lcd.setCursor(0, 0);  
  lcd.print(line2);  

  //Quanto menor o valor do delay, mais rapido o scroll  
  delay(250);  
  scroll_sup(); //Chama a rotina que executa o scroll  

  //Verifica o tamanho da string  
  tamanho = line1.length();  
  if (stringStart == tamanho)  
  {  
    stringStart = 0;  
    stringStop = 0;  
  }  
}

uint8_t modoGravacaoID(uint8_t IDgravar) {

  int p = -1;
  Serial.print("Esperando uma leitura válida para gravar #"); Serial.println(IDgravar);
  delay(2000);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Leitura concluída");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      delay(200);
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erro comunicação");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Erro de leitura");
      break;
    default:
      Serial.println("Erro desconhecido");
      break;
    }
  }

  // OK successo!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Leitura convertida");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Leitura suja");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erro de comunicação");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Não foi possível encontrar propriedade da digital");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Não foi possível encontrar propriedade da digital");
      return p;
    default:
      Serial.println("Erro desconhecido");
      return p;
  }
  
  Serial.println("Remova o dedo");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(IDgravar);
  p = -1;
  Serial.println("Coloque o Mesmo dedo novamente");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Leitura concluída");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      delay(200);
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erro de comunicação");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Erro de Leitura");
      break;
    default:
      Serial.println("Erro desconhecido");
      break;
    }
  }

  // OK successo!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Leitura convertida");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Leitura suja");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erro de comunicação");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Não foi possível encontrar as propriedades da digital");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Não foi possível encontrar as propriedades da digital");
      return p;
    default:
      Serial.println("Erro desconhecido");
      return p;
  }
  
  // OK convertido!
  Serial.print("Criando modelo para #");  Serial.println(IDgravar);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("As digitais batem!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Erro de comunicação");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("As digitais não batem");
    return p;
  } else {
    Serial.println("Erro desconhecido");
    return p;
  }   
  
  Serial.print("ID "); Serial.println(IDgravar);
  p = finger.storeModel(IDgravar);
  if (p == FINGERPRINT_OK) {
    Serial.println("Armazenado!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Erro de comunicação");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Não foi possível gravar neste local da memória");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Erro durante escrita na memória flash");
    return p;
  } else {
    Serial.println("Erro desconhecido");
    return p;
  }   
}

int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  //Encontrou uma digital!
  if (finger.fingerID == 0) {
     Serial.print("Modo Administrador!");
     
     numID++;
     modoGravacaoID(numID);
     return 0; 
  
  } else {

     digitalWrite(pinTrava, LOW);
     Serial.print("ID encontrado #"); Serial.print(finger.fingerID); 
     Serial.print(" com confiança de "); Serial.println(finger.confidence);
     delay(500);
     digitalWrite(pinTrava, HIGH);
     return finger.fingerID;
  } 
}

void scroll_sup()  
{  
  
 if(stringStart == 0 && scrollCursor > 0)
  {  
    scrollCursor--;  
    stringStop++;  
  } else if (stringStart == stringStop){  
    stringStart = stringStop = 0;  
    scrollCursor = screenWidth;  
  } else if (stringStop == line1.length() && scrollCursor == 0) {  
    stringStart++;  
  } else {  
    stringStart++;  
    stringStop++;  
  }  
}  
