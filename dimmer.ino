const int _ledPin = 9;
const int _interruptPin = 2;
const int _buttonBounceDelay = 200;
const int _dimmerInterval = 4;
const int _ldrPin = A0;

volatile bool _ledState = false;
bool _previousLedState = false;


unsigned long _previousLdrPoll = 0;


void setup() {
	Serial.begin(19200);
	//pinMode(_ledPin, OUTPUT);	// todo see if needs to be output?
	attachInterrupt(digitalPinToInterrupt(_interruptPin), ledButtonClicked, FALLING);

	Foo();
}

void Foo() {
	TCCR2B = 0x00;        //Disbale Timer2 while we set it up
	TIFR2 = 0x00;        //Timer2 INT Flag Reg: Clear Timer Overflow Flag
	TIMSK2 = 0x01;        //Timer2 INT Reg: Timer2 Overflow Interrupt Enable
	TCCR2B = 0x09;        //Timer2 Control Reg B: Timer Prescaler set to 128
}


volatile int foo = 0;
ISR(TIMER2_OVF_vect)
{
	if (foo == 5) {
		foo = 0;
		TIMSK2 = 0;
	}
	Serial.print(micros());
	Serial.println(" hurr");
	foo++;
}

void loop() {

	// Photo resistor
	unsigned long ldrPoll = millis();
	if (ldrPoll - _previousLdrPoll > 500) {
		int v = analogRead(_ldrPin);
		Serial.println(v);
		_previousLdrPoll = ldrPoll;
		if (v > 700) {
			_ledState = true;
			Serial.println(v);
			//TIMSK2 = 0x01;	// enable again
		}
		else {
			_ledState = false;
		}
	}



	if (_previousLedState != _ledState) {
		_previousLedState = _ledState;
		toggleLed(_ledState);
	}


	// Handle serial stuff
	if (Serial.available() > 0) {
		String foo = Serial.readString();
		if (foo == "on") {
			_ledState = true;
		}
		else if (foo == "off") {
			_ledState = false;
		}
	}
}


void toggleLed(bool state) {
	if (state) {
		Serial.println("Led turning on");
		dimLedOn();
	}
	else {
		Serial.println("Led turning off");
		dimLedOff();
	}
}

void dimLedOn() {
	for (int d = 0; d <= 255; d++) {
		analogWrite(_ledPin, d);
		delay(_dimmerInterval);
	}
}

void dimLedOff() {
	for (int d = 255; d >= 0; d--) {
		analogWrite(_ledPin, d);
		delay(_dimmerInterval);
	}
}


unsigned long _lastInterruptTime = 0;
void ledButtonClicked() {
	unsigned long interruptTime = millis();
	if (interruptTime - _lastInterruptTime > _buttonBounceDelay)
	{
		//Serial.println("button pressed");
		_ledState = !_ledState;
	}
	/*else {
	Serial.println("bounce...");
	}*/
	_lastInterruptTime = interruptTime;
}