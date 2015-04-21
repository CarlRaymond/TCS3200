//colorsample.ino

#include "filter.h";
#include "hsv.h";

const int PIN_LED = 13;
const int PIN_SENSOR_S2 = 2;
const int PIN_SENSOR_S3 = 3;

volatile int rawRed = 0;
volatile int maxRed = 0;
volatile int filteredRed = 0;
volatile int rawGreen = 0;
volatile int maxGreen = 0;
volatile int filteredGreen = 0;
volatile int rawBlue = 0;
volatile int maxBlue = 0;
volatile int filteredBlue = 0;
volatile int rawClear = 0;
volatile int maxClear = 0;
volatile int filteredClear = 0;

// Scale factors, initialized to reasonable values
const unsigned int SCALE = 60000;
unsigned int scaleRed = SCALE/200;
unsigned int scaleGreen = SCALE/200;
unsigned int scaleBlue = SCALE/200;

enum colorMode_t { RED, BLUE, GREEN, CLEAR };

volatile colorMode_t colorMode = RED;


volatile int mode = 0;

// Sample rate is 4ms for a full cycle of four colors, or 250 samples/sec.
// High-pass, 10Hz
Filter filterClear = Filter(HIGHPASS, 1.257, 0.5914);

// Low-pass, 40Hz
Filter filterRed = Filter(LOWPASS, 2.819, 0.2905);
Filter filterGreen = Filter(LOWPASS, 2.819, 0.2905);
Filter filterBlue = Filter(LOWPASS, 2.819, 0.2905);

// keep-on counter for led.
unsigned int ledTimer = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_SENSOR_S2, OUTPUT);
  pinMode(PIN_SENSOR_S3, OUTPUT);

  configureSensor();

  // Reset timers 1 and 2
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR2A = 0;
  TCCR2B = 0;

  // Zero the counters
  TCNT1 = 0;
  TCNT2 = 0;

  // Configure timer 1 to count external events (pulses from TCS3200)
  TCCR1B = bit(CS12) | bit(CS11) | bit(CS10);

  // Configure timer 2 to interrupt every millisecond: prescaler=128, count up to 125
  GTCCR = bit(PSRASY); // Reset prescaler
  OCR2A = 124; // Count to 125
  //OCR2A = 249;
  TIMSK2 = bit(OCIE2A); // Enable interrupt

  startTimer();

  Serial.begin(115200);
  Serial.println("Calibrating...");
  calibrate();
  Serial.print("Scale red: ");
  Serial.print(scaleRed);
  Serial.print("  Scale green: ");
  Serial.print(scaleGreen);
  Serial.print("  Scale blue: ");
  Serial.print(scaleBlue);
  Serial.println();
}


// Determines scale values
void calibrate() {

	// Get maximum counts over long interval
	cli();
	maxRed = 0;
	maxGreen = 0;
	maxBlue = 0;
	sei();

	// Wait 250ms
	delay(250);

	cli();
	scaleRed = 60000 / maxRed;
	scaleGreen = 60000 / maxGreen;
	scaleBlue = 60000 / maxBlue;
	sei();
}


void loop() {


	if (filteredClear < -30) {
		ledTimer = 20;
		PORTD |= bit(7);
	}

	if (ledTimer > 0) {
		ledTimer--;

		if (ledTimer == 0) {
			PORTD &= ~bit(7);
		}
	}

	writeResult();

}

void configureSensor() {
	switch (colorMode) {
		case RED: // Red
			digitalWrite(PIN_SENSOR_S2, LOW);
			digitalWrite(PIN_SENSOR_S3, LOW);
			digitalWrite(PIN_LED, HIGH);
			break;

		case BLUE: // Blue
			digitalWrite(PIN_SENSOR_S2, LOW);
			digitalWrite(PIN_SENSOR_S3, HIGH);
			break;

		case CLEAR: // Clear
			digitalWrite(PIN_SENSOR_S2, HIGH);
			digitalWrite(PIN_SENSOR_S3, LOW);
			digitalWrite(PIN_LED, LOW);
			break;

		case GREEN: // Green
			digitalWrite(PIN_SENSOR_S2, HIGH);
			digitalWrite(PIN_SENSOR_S3, HIGH);

	}
}

void stopTimer() {
	// Stop timer 2
	TCCR2A = 0;
	TCCR2B = 0;	
}

void startTimer() {
	// Start timer 2
  TCCR2A = bit(WGM21); // CTC mode
  TCCR2B = bit(CS20) | bit (CS22); // Prescaler 128	
}

void clearCounter() {
	// Clear counter
	TCNT1 = 0;
}

void writeResult() {

	unsigned int copyRed = rawRed;
	int copyRedFiltered = filteredRed;
	unsigned int copyGreen = rawGreen;
	int copyGreenFiltered = filteredGreen;
	unsigned int copyBlue = rawBlue;
	int copyBlueFiltered = filteredBlue;
	unsigned int copyClear = rawClear;
	int copyClearFiltered = filteredClear;

	unsigned int scaledRed = filteredRed * scaleRed;
	unsigned int scaledGreen = filteredGreen * scaleGreen;
	unsigned int scaledBlue = filteredBlue * scaleBlue;

	HSV color;
	HSV::fromScaledRGB(SCALE, scaledRed, scaledGreen, scaledBlue, color);

	if (abs(copyClearFiltered) > 30) {
		Serial.print("  C: ");
		Serial.print(copyClear);
		Serial.print("  Cf: ");
		Serial.print(copyClearFiltered);

		//	Serial.println();
		Serial.print("  R: ");
		Serial.print(copyRed);
		Serial.print("  G: ");
		Serial.print(copyGreen);
		Serial.print("  B: ");
		Serial.print(copyBlue);

		Serial.print("  Rf: ");
		Serial.print(copyRedFiltered);
		Serial.print("  Gf: ");
		Serial.print(copyGreenFiltered);
		Serial.print("  Bf: ");
		Serial.print(copyBlueFiltered);

		Serial.print("  Rs: ");
		Serial.print(scaledRed);
		Serial.print("  Gs: ");
		Serial.print(scaledGreen);
		Serial.print("  Bs: ");
		Serial.print(scaledBlue);

		Serial.print("  H: ");
		Serial.print(color.h);
		Serial.print("  S: ");
		Serial.print(color.s);
		Serial.print("  V: ");
		Serial.print(color.v);
		Serial.println();
	}
}

ISR(TIMER2_COMPA_vect) {

	// Get counter value
	unsigned int count = TCNT1;

	stopTimer();

	// Copy value
	switch(colorMode) {
		case RED:
			rawRed = count;
			if (count > maxRed) maxRed = count;
			filteredRed = filterRed.next(count);
			colorMode = GREEN;
			break;

		case GREEN:
			rawGreen = count;
			if (count > maxGreen) maxGreen = count;
			filteredGreen = filterGreen.next(count);
			colorMode = BLUE;
			break;

		case BLUE:
			rawBlue = count;
			if (count > maxBlue) maxBlue = count;
			filteredBlue = filterBlue.next(count);
			colorMode = CLEAR;
			break;

		default:
			rawClear = count;
			if (count > maxClear) maxClear = count;
			filteredClear = filterClear.next(count);
			colorMode = RED;
			break;
			}

	configureSensor();
	clearCounter();

	startTimer();

}