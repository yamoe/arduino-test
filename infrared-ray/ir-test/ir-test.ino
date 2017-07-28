#include <IRremoteModule.h>

const int irReceiverPin = A3; // use analog port
IRrecv irrecv(irReceiverPin);
decode_results decodedSignal;

void setup() {
  Serial.begin(9600);
  pinMode(irReceiverPin, INPUT);
  irrecv.enableIRIn(); // Start the receiver object
}

//void dump(void* v) {
//  decode_results* results = (decode_results*)v;
void dump(decode_results* results) {
  int count = results->rawlen;
  if (results->decode_type == UNKNOWN) {
    Serial.print("Unknown encoding: ");
  }
  else if (results->decode_type == NEC) {
    Serial.print("Decoded NEC: ");
  }
  else if (results->decode_type == SONY) {
    Serial.print("Decoded SONY: ");
  }
  else if (results->decode_type == RC5) {
    Serial.print("Decoded RC5: ");
  }
  else if (results->decode_type == RC6) {
    Serial.print("Decoded RC6: ");
  }
  else if (results->decode_type == PANASONIC) {
    Serial.print("Decoded PANASONIC: ");
  }
  else if (results->decode_type == LG) {
    Serial.print("Decoded LG: ");
  }
  else if (results->decode_type == JVC) {
    Serial.print("Decoded JVC: ");
  }
  Serial.print(results->value, HEX);
  Serial.print(" (");
  Serial.print(results->bits, DEC);
  Serial.println(" bits)");
  Serial.print("Raw (");
  Serial.print(count, DEC);
  Serial.print("): ");

  for (int i=0; i < count; i++) {
    if ((i % 2) == 1) {
      Serial.print(results->rawbuf[i] * USECPERTICK, DEC);
    }
    else {
      Serial.print(-(int)results->rawbuf[i] * USECPERTICK, DEC);
    }
    Serial.print(" ");
  }
  Serial.println("");
}

void loop() {
  if (irrecv.decode(&decodedSignal) == true) {
    if (decodedSignal.bits > 0) {
      dump(&decodedSignal);
    }
    irrecv.resume();
  }

}
