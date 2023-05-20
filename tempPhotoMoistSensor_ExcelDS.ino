#include <Wire.h> //library allows you to communicate with I2C / TWI devices 
#include <LiquidCrystal_I2C.h> //library for I2C LCD displays
volatile int value;
int exampleVariable = 0;
int sensorPin1 = A1; //MOIST
int sensorPin2 = A2; //PHOTO
int sensorPin3 = A3; //TEMP
#define sensorPin A1 ///////////
#define sensorPin A2 ///////////
#define sensorPin A3 ///////////
LiquidCrystal_I2C lcd (0x27,16,2); // set the LCD address to 0x27 for a16 chars and 2 line display
byte degree[8] = { //creates custom degree symbol on LCD scren
  B00000,
  B00111,
  B00101,
  B00111,
  B00000,
  B00000,
  B00000,
};
// Serial data variables ------------------------------------------------------
//Incoming Serial Data Array
const byte kNumberOfChannelsFromExcel = 6; 

// Comma delimiter to separate consecutive data if using more than 1 sensor
const char kDelimiter = ',';    
// Interval between serial writes
const int kSerialInterval = 50;   
// Timestamp to track serial interval
unsigned long serialPreviousTime; 

char* arr[kNumberOfChannelsFromExcel];

void setup () {
  Serial.begin (9600); // Set the serial port baud rate to 9600
  value = 0;
  lcd.init ();
  lcd.backlight (); // Light up the backlight
  lcd.setCursor (0, 0);
  lcd.print ("TEMP PHOTO MOIST"); 
  lcd.setCursor (0,1);
  lcd.print ("SENSOR");
  delay(5000); //10 seconds until loop begins
  lcd.clear (); // Clear the screen
  Serial.begin (9600); // Set the serial port baud rate to 9600
  pinMode (A1, INPUT); // Soil sensor is at A1, the mode is input
  pinMode (A2, INPUT); // Photo is at A2, the mode is input
  pinMode (A3, INPUT);
  int sensorPin = A1;
  lcd.createChar(0, degree);
}

// SENSOR INPUT CODE
void processSensors() 
{
  // Read sensor pin and store to a variable
 exampleVariable = analogRead( sensorPin1 );
  // Add any additional raw data analysis below (e.g. unit conversions)
}
// OUTGOING SERIAL DATA PROCESSING CODE----------------------------------------
void sendDataToSerial()
{
  // Send data out separated by a comma (kDelimiter)
  // Repeat next 2 lines of code for each variable sent:

  Serial.print(exampleVariable);
  Serial.print(kDelimiter);
  
  Serial.println(); // Add final line ending character only once
}

// INCOMING SERIAL DATA PROCESSING CODE----------------------------------------
void processIncomingSerial()
{
  if(Serial.available()){
    parseData(GetSerialData());
  }
}


// OUTGOING SERIAL DATA PROCESSING CODE----------------------------------------
void processOutgoingSerial()
{
   // Enter into this only when serial interval has elapsed
  if((millis() - serialPreviousTime) > kSerialInterval) 
  {
    // Reset serial interval timestamp
    serialPreviousTime = millis(); 
    sendDataToSerial(); 
  }
}

// Gathers bytes from serial port to build inputString
char* GetSerialData()
{
  static char inputString[64]; // Create a char array to store incoming data
  memset(inputString, 0, sizeof(inputString)); // Clear the memory from a pervious reading
  while (Serial.available()){
    Serial.readBytesUntil('\n', inputString, 64); //Read every byte in Serial buffer until line end or 64 bytes
  }
  return inputString;
}

// Seperate the data at each delimeter
void parseData(char data[])
{
    char *token = strtok(data, ","); // Find the first delimeter and return the token before it
    int index = 0; // Index to track storage in the array
    while (token != NULL){ // Char* strings terminate w/ a Null character. We'll keep running the command until we hit it
      arr[index] = token; // Assign the token to an array
      token = strtok(NULL, ","); // Conintue to the next delimeter
      index++; // incremenet index to store next value
    }
}

void loop () {
  int reading = analogRead(sensorPin3); //Get a reading from the temperature sensor: ///////////////////
  // Gather and process sensor data
  processSensors();
  // Read Excel variables from serial port (Data Streamer)
  processIncomingSerial();
  // Process and send data to Excel via serial port (Data Streamer)
  processOutgoingSerial();

  delay (1000); // Delay 0.5S
  lcd.clear (); // clear screen
    lcd.setCursor (0, 0);
       lcd.print ("TEMP PHOTO MOIST"); //
  value = analogRead (A1); // Read the value of the moisture sensor
       lcd.setCursor (12, 1);
       lcd.print (value);
       Serial.print(value);
       Serial.print(", ");
  value = analogRead (A2); // Read the value of the photo sensor
       lcd.setCursor (6, 1);
       lcd.print (value);
       Serial.print(value);
       Serial.print(", ");
  value = analogRead (A3); // Read the value of the temp sensor
       int voltage = value * (5000 / 1024.0); // Convert the reading into voltage:
       int temperature = voltage / 10; // Convert the voltage into the temperature in degree Celsius:
       lcd.setCursor (0, 1);
       lcd.print (temperature);
       Serial.print(temperature);
       Serial.println("");
       lcd.write(byte(0)); //arduino writes custom character to LCD
       lcd.print("C");
       delay (1000); // Delay between readings
}
