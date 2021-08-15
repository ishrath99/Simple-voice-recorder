
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define pass (void)0
#define SD_CSPin 10
#define S_mode 8
#define S_playorrec 9
#define S_playnext A3
#define Reset A2

LiquidCrystal_I2C lcd(0x27, 16, 4);

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


void setup() {
  pinMode(S_playorrec, INPUT);
  pinMode(S_mode, INPUT);
  pinMode(S_playnext, INPUT);
  pinMode(Reset, INPUT);
  DDRD= 255;

  setupAnalogRead();
  
  state2 = digitalRead(S_mode);

  Serial.begin(9600);

  //intializing lcd
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("VOICE RECORDER");
  delay(1000);
  lcd.clear();

  lcd.println("loading SD");
  delay(1000); 
  if (!SD.begin(SD_CSPin)) {
    lcd.clear();
    lcd.println("SD Error");
    delay(1000);
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
        lcd.println("file error");
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
       
        myFile.println(AnalogRead());
        t1 = micros();
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
            PORTD= temp.toInt();         // 8 bit DAC input
            t1 = micros();
          }
          //mataching the input sampling rate
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

void  setupAnalogRead(){
  ADMUX |= (1<<REFS0)|(1<<ADLAR);   //VCC as reference & left justified ADC(8 bits) & select ADC0 as input
  
  ADCSRA |= (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);    //enable ADC & set prescaler to 128 (16000000/128=125kHz)
  
  DDRC &= ~(1<<DDC0);   //select ADC0 as input
}
  
int AnalogRead(){
  ADCSRA = ADCSRA | (1 << ADSC);  //// Start an ADC conversion
  
  // Wait until the ADSC bit has been cleared
  while(ADCSRA & (1 << ADSC));
  
  return ADCH;  
}
