
#include <MCP_DAC.h>
#include <SD.h>
#include <SPI.h>
#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>

#define pass (void)0
#define SD_CSPin 4
#define S_mode 5
#define S_playorrec 6
#define S_playnext 3
#define Reset 2
#define DACpin 10

MCP4921 myDAC;
LiquidCrystal_I2C lcd(0x20, 16, 2);

long int samplingFreq = 8000;            // sampling frequency= 8kHz
long Delay = 1000000 / samplingFreq;      //sampling interval in microseconds

int state2 = 0;
int filec = 0;

File myFile, testFile;

int fileCount = 0;

char file_name[11] = "";
int file_number = 0;
char filePrefixname[4] = "voi";
char exten[5] = ".txt";

int file_numberp = 0;

const int mic_pin = A0;


void setup() {
  pinMode(S_playorrec, INPUT);
  pinMode(S_mode, INPUT);
  pinMode(S_playnext, INPUT);
  pinMode(Reset, INPUT);
  pinMode(mic_pin, INPUT);
  //pinMode(DACpin, OUTPUT);

  ADCSRA &= ~(1 << ADPS2);     //set ADC Prescaler Select Bits to 011
  ADCSRA |= (1 << ADPS1);      // division factor = 8
  ADCSRA |= (1 << ADPS0);

  //  TCCR2B &= ~(1<<WGM22);
  //  TCCR2A |= (1<<WGM21);
  //  TCCR2A &= ~(1<<WGM20);
  //  TCCR2A |= (1<<COM2A1);
  //  TCCR2A &= ~(1<<COM2A0);
  //  TCCR2A |= (1<<COM2B1);
  //  TCCR2A &= ~(1<<COM2B0);
  //
  //  TCCR2B |= (1<<CS22);
  //  TCCR2B &= ~(1<<CS21);
  //  TCCR2B &= ~(1<<CS20);

  state2 = digitalRead(5);

//  Serial.begin(9600);
  SD.begin(SD_CSPin);
  myDAC.begin(DACpin);

  //intializing lcd
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("VOICE RECORDER");
  delay(1000);
  lcd.clear();

  lcd.println("loading SD");
  delay(500);
  if (!SD.begin(SD_CSPin)) {
    lcd.clear();
    lcd.println("An Error!!");
    delay(500);
  }
  while (!SD.begin(SD_CSPin)) {
    lcd.clear();
    lcd.print(".");
    delay(500);
  }
  File root = SD.open("/");
  fileCount = getFileCount(root);
  root.close();
  file_number = fileCount;
}


void loop() {
  long t0, t1;
  if (digitalRead(Reset) == HIGH) {
    File root = SD.open("/");
    reset(root);
    root.close();
    file_number = 0;
    file_numberp = 0;
    lcd.clear();
    lcd.println("SD reset!");
    delay(1000);
  }

  if (digitalRead(S_mode) == LOW) {
    if (state2 == LOW) {
      lcd.clear();
      lcd.println("Record mode");
      delay(500);
      state2 = HIGH;
    }

    if (digitalRead(S_playorrec) == HIGH) {
      char fileSlNum[4] = "";
      itoa(file_number, fileSlNum, 10);
      strcat(file_name, filePrefixname);
      strcat(file_name, fileSlNum);
      strcat(file_name, exten);
      myFile = SD.open(file_name, FILE_WRITE);

      if (!myFile) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.println("file error!");
        delay(1000);
      }
      else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.println(file_name);
        lcd.setCursor(0, 1);
        lcd.println("Recording");
      }

      while (true) {
        t0 = micros();
        if (digitalRead(S_playorrec) == LOW) {
          myFile.close();
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.println(file_name);
          lcd.setCursor(0, 1);
          lcd.println("Recorded");
          delay(1000);
          file_number++;
          file_name[0] = '\0';
          break;
        }
        int data = analogRead(mic_pin);
        myFile.println(data);
        t1 = micros();
        //Serial.println(t1-t0);
        if (t1 - t0 < Delay) {
          delayMicroseconds(Delay + t0 - t1);
        }
      }
    }

  } else {

    if (state2 == HIGH) {
      File root = SD.open("/");
      filec = getFileCount(root);
      root.close();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.println("Play mode");
      state2 = LOW;
    }

    if (digitalRead(S_playorrec) == HIGH) {

      if (filec != 0) {
        String temp;
        char fileSlNump[4] = "";
        itoa(file_numberp, fileSlNump, 10);
        strcat(file_name, filePrefixname);
        strcat(file_name, fileSlNump);
        strcat(file_name, exten);
        testFile = SD.open(file_name);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.println(file_name);
        lcd.setCursor(0, 1);
        lcd.println("Playing");

        while (testFile.available()) {
          t0 = micros();
          if (digitalRead(S_playnext) == HIGH) {
            delay(1000);
            break;
          } else if (digitalRead(S_playorrec) == LOW) {
            if (digitalRead(S_mode) == LOW) {
              break;
            } else if (digitalRead(S_playnext) == HIGH) {
              break;
            }
            pass;
          } else {
            temp = testFile.readStringUntil('\n');
            myDAC.analogWrite(temp.toInt());
            t1 = micros();
          }
          //Serial.println(t1-t0);
          if (t1 - t0 < Delay) {
            delayMicroseconds(Delay + t0 - t1);
          }
        }
        testFile.close();

        if (filec == file_numberp + 1) {
          file_numberp = 0;
        } else {
          file_numberp++;
        }
        file_name[0] = '\0';



      }
      else {
        lcd.clear();
        lcd.println("No files in SD");
        delay(1000);
      }
    }
  }

}

int getFileCount(File root) {
  int fileCountOnSD = 0;
  while (true) {
    File entry =  root.openNextFile();
    if (! entry) {
      entry.close();
      break;
    }
    fileCountOnSD++;
    entry.close();
  }
  return fileCountOnSD;
}

void reset(File root) {
  while (true) {
    File entry = root.openNextFile();
    if (!entry) {
      entry.close();
      break;
    }
    SD.remove(entry.name());
    entry.close();
  }
}
