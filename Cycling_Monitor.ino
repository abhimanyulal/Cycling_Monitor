#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

const int Up_buttonPin = 51;
static int buttonState = 0;
static int Km = 0;
static int M = 0;
static int Hm = 0;
static int hour = 0;
static int minutes = 0;
static int seconds = 0;

int keypad_pin = A0;
int keypad_value = 0;
int keypad_value_old = 0;
int buttonPushCounter = 0;
int up_buttonState = 0;
int up_lastButtonState = 0;
int down_buttonState = 0;
int down_lastButtonState = 0;

float prevTouch = 0;
float curTouch = 0;
float curSpeed = 0;
float Distance = 0;
float cal = 0;

boolean bPress = false;
boolean menuA = false;
boolean menuB = false;
boolean menuC = false;
boolean menuD = false;

static long Timer = 0;
static long distance = 0;
static double Speed = 0;

char btn_push;

byte mainMenuPage = 1;
byte mainMenuPageOld = 1;
byte mainMenuTotal = 4;

void setup()
{
  Serial.begin (9600);
  pinMode( Up_buttonPin , INPUT_PULLUP);
  lcd.begin(16, 2);
  MainMenuDisplay();

}
void checkUp()
{
  float timeLapse = 0;
  float betweenTouchDistance = 2.12;

  up_buttonState = digitalRead(Up_buttonPin);
  if (up_buttonState != up_lastButtonState) {
    if (up_buttonState == LOW)  {
      bPress = true;
      buttonPushCounter++;
      Serial.println("on");
      Serial.print("number of button pushes: ");
      Serial.println(buttonPushCounter);

      curTouch = millis();
      timeLapse = curTouch - prevTouch;
      curSpeed = (betweenTouchDistance / timeLapse) * 3600;
      Serial.print (curSpeed);
      Serial.print(timeLapse);
      prevTouch = curTouch;

    } else {
      Serial.println("off");
    }
  }
  Distance = buttonPushCounter * 2.12;
  M = Distance;
  Km = M / 1000;
  M = M % 1000;
  up_lastButtonState = up_buttonState;
  if ( bPress) {
    bPress = false;
  }
}
void stopWatch () {
  Timer = millis() / 1000;
  minutes = Timer / 60;
  seconds = Timer % 60;
  hour = minutes / 60;
  minutes = minutes % 60;

  Serial.print ("h; ");
  Serial.print (hour);
  Serial.print ("m; ");
  Serial.print (minutes);
  Serial.print ("s; ");
  Serial.print (seconds);
  Serial.print ("\n");
}
void calories()
{
  if (curSpeed <= 7) {
    cal = cal + 0;
  }
  else if (curSpeed <= 22)
  {
    cal = cal + (472 / Timer);
  }
  else if (curSpeed <= 25)
  {
    cal = cal + (590 / Timer);
  }
  else if (curSpeed <= 30)
  {
    cal = cal + (708 / Timer);
  }
  else
  {
    cal = cal + (944 / Timer);
  }
}

void CSpeed()
{
  Speed = Distance / Timer;
}
void MenuA()
{
  char timeStr [8];
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Time");

  while (ReadKeypad() != 'L')
  {
    stopWatch();
    checkUp();
    CSpeed();
    calories();
    lcd.setCursor (0, 1);
    sprintf (timeStr, "%02d:%02d:%02d", hour, minutes, seconds);
    lcd.print (timeStr);
  }
}
void MenuB()
{
  char distStr [9];
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Distance");

  while (ReadKeypad() != 'L')
  {
    checkUp();
    stopWatch();
    CSpeed();
    calories();
    lcd.setCursor (0, 1);
    sprintf (distStr, "%02d.%03d Km", Km, M);
    lcd.print (distStr);
  }
}
void MenuC()
{

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("");

  while (ReadKeypad() != 'L')
  {
    CSpeed();
    checkUp();
    stopWatch();
    calories();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print (Speed);
    lcd.setCursor(7, 0);
    lcd.print ("Avg Kmph");
    Serial.print (Speed);
    lcd.setCursor (0, 1);
    lcd.print (curSpeed);
    lcd.setCursor (7, 1);
    lcd.print ("Cur Speed");
  }
}
void MenuD()
{
  char calStr [5];
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Inside Menu D");

  while (ReadKeypad() != 'L')
  {
    calories();
    CSpeed();
    stopWatch();
    checkUp();
    lcd.setCursor(0, 1);
    sprintf(calStr, "%05d", cal);
    lcd.print (cal);
    lcd.setCursor(13, 1);
    lcd.print ("Cal");
    Serial.print (calStr);

  }
}
void MainMenuDisplay()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  switch (mainMenuPage)
  {
    case 1:
      lcd.print("Time");
      break;
    case 2:
      lcd.print("Distance");
      break;
    case 3:
      lcd.print("Speed");
      break;
    case 4:
      lcd.print("Calories");
      break;
  }
}
void MainMenuBtn()
{
  WaitBtnRelease();
  if (btn_push == 'U')
  {
    mainMenuPage++;
    if (mainMenuPage > mainMenuTotal)
      mainMenuPage = 1;
  }
  else if (btn_push == 'D')
  {
    mainMenuPage--;
    if (mainMenuPage == 0)
      mainMenuPage = mainMenuTotal;
  }

  if (mainMenuPage != mainMenuPageOld) //only update display when page change
  {
    MainMenuDisplay();
    mainMenuPageOld = mainMenuPage;
  }
}
char ReadKeypad()
{
  /* Keypad button analog Value
    no button pressed 1023
    select  741
    left    503
    down    326
    up      142
    right   0
  */
  keypad_value = analogRead(keypad_pin);

  if (keypad_value < 100)
    return 'R';
  else if (keypad_value < 200)
    return 'U';
  else if (keypad_value < 400)
    return 'D';
  else if (keypad_value < 600)
    return 'L';
  else if (keypad_value < 800)
    return 'S';
  else
    return 'N';

}

void WaitBtnRelease()
{
  while ( analogRead(keypad_pin) < 800) {}
}
void loop()
{
  calories();
  stopWatch();
  checkUp();
  CSpeed();
  btn_push = ReadKeypad();

  MainMenuBtn();

  if (btn_push == 'S') //enter selected menu
  {
    WaitBtnRelease();
    switch (mainMenuPage)
    {
      case 1:
        MenuA();
        break;
      case 2:
        MenuB();
        break;
      case 3:
        MenuC();
        break;
      case 4:
        MenuD();
        break;
    }

    MainMenuDisplay();
    WaitBtnRelease();
  }
}
