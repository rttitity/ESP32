void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  pinMode(D8, OUTPUT);
}
int i = 5;
int x = 0;
void loop() {

  if(i==5){
    digitalWrite(D1,x);
    if(x==0){
      x = 1;
    }
    else{
      x = 0;
    }
    i =  1;
  }
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(D4, LOW);
  digitalWrite(D5, LOW);
  digitalWrite(D6, LOW);
  digitalWrite(D7, LOW);
  digitalWrite(D8, LOW);
     
  delay(300);
  i = i+1;        

   if(i==5){
    digitalWrite(D1,x);
    if(x==0){
      x = 1;
    }
    else{
      x = 0;
    }
    i =  1;
  }
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(D4, HIGH);
  digitalWrite(D5, HIGH);
  digitalWrite(D6, HIGH);
  digitalWrite(D7, HIGH);
  digitalWrite(D8, HIGH); 
  delay(300);
  i = i+1;
}
