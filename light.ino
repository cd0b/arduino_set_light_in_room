#define ON 1
#define OFF 0
#define THOLD_TOUCH 3

static int turn = ON;
static int touch = 49;




static const int desiredLight = 300;
static int synLight = 0; // syncthetic light.
static int nsens = 10000;
static double sensitivity = 0;


#define NSAMPLE 100
#define NSENS 10000



#define setLight() analogWrite(3, map(synLight, 0, 1023, 0, 255));
#define getLight() analogRead(A5);
#define getTouch() analogRead(A0);





static void collectSamples(int* samples);
static int calcSamples(int* samples);
static int calcFromSamples();
static double calcSensitivity();
static int needLight();
static void switchTurn();
static int readTouch();











void setup() {

  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(A5, INPUT);
  pinMode(3, OUTPUT);
  analogReference(EXTERNAL);
  
}

void loop() {

  if(turn == ON) {
    int need = needLight();
    adjLight(need);
  } else {
    adjLight(-1023);
  }

  int touch = readTouch();
  if(touch == 1) switchTurn();

  delay(100);


}




static int readTouch() {

  
  int analogTouch = getTouch();

  if(analogTouch > THOLD_TOUCH) {
    Serial.println(analogTouch);
    if(touch > 50) {
      touch = 0;
      return 1;
    }
  }

  ++touch;
  if(touch == 0xffffffff) touch = 49;
  return 0;
  
}




static void switchTurn() {
  if(turn == ON) turn = OFF;
  else turn = ON;
}






static void adjLight(int need) {

  synLight += need;
  
  
  if(synLight > 1023) synLight = 1023;
  else if(synLight < 0) synLight = 0;
  setLight();
  
}








static void collectSamples(int* samples) {
  for(int i = 0; i < NSAMPLE; ++i)
    *(samples+i) = getLight();  
}




static int calcSamples(int* samples) {
  int sum = 0;
  for(int i = 0; i < NSAMPLE; ++i) {
    sum += (*(samples+i)) / NSAMPLE;  
  }  
  return sum;
}




static int calcFromSamples() {
  int samples[NSAMPLE];
  collectSamples(samples);
  return calcSamples(samples);
}




static double calcSensitivity() {

  if(nsens < NSENS) {
    ++nsens;  
    return sensitivity;
  } else {
    nsens = 0;
  }

  int temp = synLight;
  
  synLight = 1023;
  setLight(); // make light max.

  int lightMax = calcFromSamples();
  

  synLight = 0;
  setLight(); // make light 0.
  
  int lightMin = calcFromSamples(); // get light in room.
  
  sensitivity = ((lightMax-lightMin) / 102.4);

  synLight = temp;
  setLight();

  return sensitivity;
}






static int needLight() {

  int light = getLight();
  
  double sensitivity = calcSensitivity();

  int need = ((desiredLight - light) / sensitivity) / 10;
  return need;
  
}
