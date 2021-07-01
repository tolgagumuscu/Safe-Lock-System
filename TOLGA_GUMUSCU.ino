//TOLGA GUMUSCU S017901
//EE321 TERM PROJECT
#include <Key.h>
#include <Keypad.h>
#include <LiquidCrystal.h>

#define length 4
//this represents my passwords' lengths
//implementing my pin outputs
LiquidCrystal lcd(22, 23, 24, 25, 26, 27);
int led     = 13;
int motorA1 = 29; int motorA2 = 30;

int motorB1 = 32; int motorB2 = 33;

int successful_attempt = 0; int unsuccessful_attempt = 0;

char password[4];
int i = 20;

const byte row    = 4; const byte column = 3;

byte data_count = 0;

int position = 0;
int index    = 0;
int SFA      = 0;
int seconds  = 0;
int a_check  = 0;
int b_check  = 0;

byte master_count = 0;

char Data[length];
//Data is my input, kinda works like Scanner in Java.
char Master[length]   = {'1', '2', '3', '4'};
char SectionA[length] = {'2', '2', '2', '2'};
char SectionB[length] = {'3', '3', '3', '3'};
char Bait[length]     = {'9', '9', '9', '9'};
char AScreen[1] = {'1'};
char BScreen[1] = {'1'};
char ChangePassword[1] = {'1'};

char pressKey;
char trial;
//definition of my keypad button order
char keys[row][column] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
//myKeyPad pins
byte rowPin[row]       = {4, 3, 2, 8};
byte columnPin[column] = {7, 6, 5};
Keypad keypad = Keypad( makeKeymap(keys), rowPin, columnPin, row, column);

void setup() {
  Serial.begin(9600);
  pinMode(motorA1, OUTPUT);
  pinMode(motorA2, OUTPUT);
  pinMode(motorB1, OUTPUT);
  pinMode(motorB2, OUTPUT);
  pinMode(led    , OUTPUT);
  //I am activating my pins in here

  //Opening Screen
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Welcome to Hotel Safe");
  lcd.setCursor(0, 1);
  lcd.print("Safe "); delay(300);
  lcd.println("3->"); delay(300);
  lcd.println("2->"); delay(300);
  lcd.println("1")  ;   delay(300);
  lcd.clear();

  cli();

  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 15624;
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS12) | (1 << CS10);

  sei();
}

void timer_start() {
  TCNT1 = 0;
  TIMSK1 |= (1 << OCIE1A);
}
void timer_stp() {
  TCNT1 = 0;
  seconds = -1;
  TIMSK1 |= (0 << OCIE1A);
}

ISR(TIMER1_COMPA_vect)
{
  seconds++;
  if (seconds < 15 && SFA != 0) {
    timer_start();
  } else {
    SFA = 0;
    seconds = -1;
    timer_stp();
  }
}


void loop() {
  //I code my project using GOTO function
  //That helped me to implement all of my code in "void loop()"
MainMenuScreen:
  lcd.clear(); lcd.print("MainMenu-1,2,3,4");
  clearData();
  //MenuSelection();
  delay(500);

  trial = keypad.waitForKey();

AScreen:
  if (trial == '1') {
    clearData(); lcd.clear();
    lcd.print("Enter Password");
    Password();
    if (PasswordChecker(Master) == 1 || PasswordChecker(SectionA) == 1) {
      //we accept master password or side password
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Sect.A opening");
      lcd.setCursor(0,1);
      lcd.print("/locking");
      //timer_stp();
      increase_success();
      SFA = 0;
      if (a_check == 0) {
        digitalWrite(motorA2, HIGH);
        delay(778);
        //in order to rotate my motor
        //I calculated and experimented some values
        //and I found out that at 778milisecond it rotates
        //exactly 2 times.
        digitalWrite(motorA2, LOW);
        delay(500);
      }

      if (a_check == 1) {
        digitalWrite(motorA1, HIGH);
        delay(778);
        digitalWrite(motorA1, LOW);
        a_check = 0;
      }
      a_check++;
      delay(500);

      goto MainMenuScreen;
    }
    if (PasswordChecker(Bait) == 1) {
      clearData();
      //timer_stp();
      increase_success();
      SFA = 0;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Sect.A opening");
      lcd.setCursor(0,1);
      lcd.print("/locking");
      //If bait password activated, I have to blink my Led.
      digitalWrite(led, HIGH);
      delay(300);
      digitalWrite(led, LOW);
      delay(300);
      digitalWrite(led, HIGH);
      delay(300);
      digitalWrite(led, LOW);

      if (a_check == 0) {
        digitalWrite(motorA2, HIGH);
        delay(778);
        digitalWrite(motorA2, LOW);
        delay(500);
      }
      if (a_check == 1) {
        digitalWrite(motorA1, HIGH);
        delay(778);
        digitalWrite(motorA1, LOW);
        a_check = 0;
      }
      a_check++;

      goto MainMenuScreen;
    }
    else {
      lcd.clear(); clearData();
      lcd.print("Wrong Password");
      SFA++;
      delay(1000);
      if (SFA != 3) {
        lcd.clear();
        decrease_success();
        //timer_start();
        goto AScreen;
      }

      lcd.clear();
      goto MasterLockScreen;
    }
  }
BScreen:
  if (trial == '2') {
    clearData(); lcd.clear();
    lcd.print("Enter Password");
    Password();
    if (PasswordChecker(Master) == 1 || PasswordChecker(SectionB) == 1) {
      increase_success();
      // timer_start();
      SFA = 0;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Sect.B opening");
      lcd.setCursor(0,1);
      lcd.print("/locking");

      if (b_check == 0) {
        digitalWrite(motorB2, HIGH);
        delay(778);
        digitalWrite(motorB2, LOW);
        delay(500);
      }
      if (b_check == 1) {
        digitalWrite(motorB1, HIGH);
        delay(778);
        digitalWrite(motorB1, LOW);
        b_check = 0;
      }
      b_check++;
      goto MainMenuScreen;
    }
    if (PasswordChecker(Bait) == 1) {
      clearData();
      increase_success();
      //timer_start();
      SFA = 0;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Sect.B opening");
      lcd.setCursor(0,1);
      lcd.print("/locking");
      digitalWrite(led, HIGH);
      delay(300);
      digitalWrite(led, LOW);
      delay(300);
      digitalWrite(led, HIGH);
      delay(300);
      digitalWrite(led, LOW);

      if (b_check == 0) {
        digitalWrite(motorB2, HIGH);
        delay(778);
        digitalWrite(motorB2, LOW);
        delay(500);
      }
      if (b_check == 1) {
        digitalWrite(motorB1, HIGH);
        delay(778);
        digitalWrite(motorB1, LOW);
        b_check = 0;
      }
      b_check++;
      goto MainMenuScreen;
    }
    else {
      lcd.clear(); clearData();
      lcd.print("Wrong Password");
      // timer_start();
      delay(500);
      unsuccessful_attempt++;
      SFA++;
      delay(1000);
      if (SFA == 3) {
        lcd.clear();
        //lcd.print("qqq");
        goto MasterLockScreen;
      }
      lcd.clear();
      goto BScreen;
    }
  }
ChangePassword:
  if (trial == '3') {
    clearData();
    lcd.clear(); lcd.print("Master P. for CP");
    //Master Password needed to edit passwords
    Password();
    if (PasswordChecker(Master) == 1) {
      increase_success();
      //timer_start();
      SFA = 0;
      clearData();
      delay(500); lcd.clear(); lcd.print("CP 1M 2A 3B 4T"); MenuSelection();
      if (Data[0] == '1') {
        lcd.clear();
        lcd.print("Enter New Pass M");
        clearData();
        Password();
        for (int i = 0; i < 4; i++) {
          Master[i] = Data[i];
          //in here I am changing the passwords with
          //the user input
        }
        lcd.clear(); lcd.print("Pass is changed"); delay(1500); goto MainMenuScreen;
      }
      if (Data[0] == '2') {
        lcd.clear();
        lcd.print("Enter New Pass A");
        clearData();
        Password();
        for (int i = 0; i < 4; i++) {
          SectionA[i] = Data[i];
        }
        lcd.clear(); lcd.print("Pass is changed"); delay(1500); goto MainMenuScreen;
      }
      if (Data[0] == '3') {
        lcd.clear();
        lcd.print("Enter New Pass B");
        clearData();
        Password();
        for (int i = 0; i < 4; i++) {
          SectionB[i] = Data[i];
        }
        lcd.clear(); lcd.print("Pass is changed"); delay(1500); goto MainMenuScreen;
      }
      if (Data[0] == '4') {
        lcd.clear();
        lcd.print("Enter New Pass T");
        clearData();
        Password();
        for (int i = 0; i < 4; i++) {
          Bait[i] = Data[i];
        }
        lcd.clear(); lcd.print("Pass is changed"); delay(1500); goto MainMenuScreen;
      }

    }
    else {
      lcd.clear(); clearData();
      lcd.print("Wrong Password");
      //timer_start();
      unsuccessful_attempt++;
      SFA++;
      if (SFA == 3) {
        goto MasterLockScreen;
      }
      goto ChangePassword;
    }
  }
  //Unsuccessful and Successful
  if (trial == '4') {
    //This screen reports my past attempts
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(successful_attempt); lcd.print(" successful");
    lcd.setCursor(0, 1);
    lcd.print(unsuccessful_attempt); lcd.print(" unsuccessful");
    delay(1500); goto MainMenuScreen;
  }
TimeLockScreen:
  //If user enters wrong password rather than master password
  //has to wait 20 seconds in order to continue
  lcd.clear();
  time_screen();
  goto MasterLockScreen;
MasterLockScreen:
  if (SFA == 3) {
    SFA = 0;
  }
  lcd.clear();
  lcd.print("System Locked MP");
  Password();
  PasswordChecker(Master);
  if (PasswordChecker(Master) == 0) {
    goto TimeLockScreen;
    //if user enters wrong password rather than Master Password
    //has to wait in TimeLockScreen
  }
}
void Password() {
  data_count = 0;
  //This void fuction helps me to scan my users input
  //I hold it on a temporary array
  //When I store it in array, my workload decreased.
  //I can easily push my inputs to this temp array
  while (1) {
    pressKey = keypad.getKey();
    if (pressKey) {
      Data[data_count] = pressKey;
      lcd.setCursor(data_count, 1);
      lcd.print("*");
      data_count++;
      if (data_count > 3) {
        break;
      }
    }
  }

}
void MenuSelection() {
  data_count = 0;
  //Same as Password()
  //Only difference is the array size
  //I only store 1 item to select my menu option

  while (1) {
    pressKey = keypad.getKey();
    if (pressKey) {
      //Data[data_count] = pressKey;
      lcd.setCursor(7, 1);
      lcd.print(pressKey);
      data_count++;
      if (data_count > 0) {
        break;
      }
    }
    clearData();
  }

}
void clearData() {
  //This void function helps me to
  //delete my temp password array
  while (data_count != 0) {
    Data[data_count--] = 0;
  }
  return;
}

bool PasswordChecker(char tolga[length]) {
  //With the help of this bool function
  //I return "1" or "0" in the end
  //This helps me out to use my if conditions easily
  for (int i = 0; i < length; i++) {
    if (Data[i] != tolga[i]) {
      return false;
      clearData();
      SFA++;
    }
  } return true;
  clearData();
}

bool MenuSelectionChecker(char tolga[1]) {
  if (Data[0] != tolga[0]) {
    return false;
    clearData();
  }
  return true;
  clearData();
}

void increase_success() {
  successful_attempt++;
}
void decrease_success() {
  unsuccessful_attempt++;
}
void SFA_increase() {
  SFA++;
}
void time_screen() {
  for (int i = 20; i >= 0; i--) {
    lcd.print("No access "); lcd.print(i); lcd.print(" sec");
    delay(1000);
    lcd.clear();
  }
}
