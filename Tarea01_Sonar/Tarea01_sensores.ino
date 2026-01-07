const int TRIG_PIN = 9; //elige los pins PWM a usar
const int ECHO_PIN = 10;
long duration; //definición de variables
int distance;

void setup() {
  pinMode(TRIG_PIN, OUTPUT); //Asigna a TRIG_PIN como pin de salida
  pinMode(ECHO_PIN, INPUT); //Asigna a ECHO_PIN como pin de entrada
  Serial.begin(9600);//Inicia comunicación por el puerto serial
}

void loop() {
  digitalWrite(TRIG_PIN , LOW); //Limpia al TRIG_PIN
  delayMicroseconds(2);

  //Pone al TRIG_PIN en HIGH por 10 microsegundos 
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN , LOW);

  //Calcula el tiempo de viaje de la onda sonora en microsegundos
  duration = pulseIn(ECHO_PIN, HIGH); //Lee el ECHO_PIN
  distance = duration * 0.034/2; //Cálculo de la distancia

  //Imprime la distancia en el SerialMonitor
  Serial.print("Distance: ");
  Serial.println(distance);
  delayMicroseconds(500);
}