
//  FEZTABLE - A basic 8bit wavetable oscillator for my Kosmo.
//  designed to be a sssssimple as possible to allow for easy modification.
//
//      __*__
//     /  /  \     MY NAME-PLATE MEDALLION SAYS NEVER TRUST
//    /   \   \    A FEZ-9000
//   /    /\   \   
//  /           \
// |_____________|



#define ARDUINO_CLOCK_SPEED_HZ (16000000)
#define PRESCALER (1)
#define SAMPLE_RATE (32678) // seems to be fine at 48khz
#define COMPARE_MATCH_VALUE ((ARDUINO_CLOCK_SPEED_HZ / (PRESCALER * SAMPLE_RATE))-1)
#define WAVE_TABLE_SIZE 128

#define PWM_OUTPUT_VALUE(x) (OCR2B = OCR2A = (127 + (x)));

unsigned int sr_timer_ctr;
float current_freq = 16.35;


/// THE WAVE TABLES, they are each 128 entries, feel free to change these.
signed char WAV_TABLE1[128] = {-128, -10, -11, -12, -13, -14, -15, -16, -17, -18, -19, -19, -20, -21, -22, -23, -24, -25, -26, -27, -27, -27, -27, -27, -27, -27, -27, -26, -25, -24, -22, -20, -16, -11, -3, 60, 107, 113, 115, 117, 118, 119, 119, 119, 119, 119, 119, 119, 119, 118, 118, 117, -6, -9, -11, -13, -16, -28, -27, -22, -21, -21, -21, -21, -21, -21, -21, -34, -39, -41, -44, -48, -53, -55, -56, -57, -58, -59, -59, -58, -56, -53, -51, -49, -46, -43, -43, -44, -47, -48, -50, -51, -51, -51, -50, -50, -49, -46, 59, 58, 57, 55, 55, 55, 55, 55, 52, 50, 49, 48, 48, 48, 48, 49, 49, 52, 54, 52, 26, 13, 5, 0, -3, -7, -8, -10, -11, -12};
signed char WAV_TABLE2[128] = {-127, -125, -123, -121, -119, -117, -115, -113, -111, -109, -107, -105, -103, -101, -99, -97, -95, -93, -91, -89, -87, -85, -83, -81, -79, -77, -75, -73, -71, -69, -67, -65, -63, -61, -59, -57, -55, -53, -51, -49, -47, -45, -43, -41, -39, -37, -35, -33, -31, -29, -27, -25, -23, -21, -19, -17, -15, -13, -11, -9, -7, -5, -3, -1, 0, 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 49, 51, 53, 55, 57, 59, 61, 63, 65, 67, 69, 71, 73, 75, 77, 79, 81, 83, 85, 87, 89, 91, 93, 95, 97, 99, 101, 103, 105, 107, 109, 111, 113, 115, 117, 119, 121, 123, 125};
signed char WAV_TABLE3[128] = {-127, -123, -119, -115, -111, -107, -103, -99, -95, -91, -87, -83, -79, -75, -71, -67, -63, -59, -55, -51, -47, -43, -39, -35, -31, -27, -23, -19, -15, -11, -7, -3, 0, 3, 7, 11, 15, 19, 23, 27, 31, 35, 39, 43, 47, 51, 55, 59, 63, 67, 71, 75, 79, 83, 87, 91, 95, 99, 103, 107, 111, 115, 119, 123, 127, 123, 119, 115, 111, 107, 103, 99, 95, 91, 87, 83, 79, 75, 71, 67, 63, 59, 55, 51, 47, 43, 39, 35, 31, 27, 23, 19, 15, 11, 7, 3, 0, -3, -7, -11, -15, -19, -23, -27, -31, -35, -39, -43, -47, -51, -55, -59, -63, -67, -71, -75, -79, -83, -87, -91, -95, -99, -103, -107, -111, -115, -119, -123};
signed char WAV_TABLE4[128] = {0,6,12,18,24,30,36,42,48,54,59,65,70,75,80,85,89,94,98,102,105,108,112,114,117,119,121,123,124,125,126,126,126,126,126,125,124,123,121,119,117,114,112,108,105,102,98,94,89,85,80,75,70,65,59,54,48,42,36,30,24,18,12,6,0,-6,-12,-18,-24,-30,-36,-42,-48,-54,-59,-65,-70,-75,-80,-85,-89,-94,-98,-101,-105,-108,-111,-114,-117,-119,-121,-123,-124,-125,-126,-126,-126,-126,-126,-125,-124,-123,-121,-119,-117,-114,-112,-108,-105,-102,-98,-94,-89,-85,-80,-75,-70,-65,-59,-54,-48,-42,-36,-30,-24,-18,-12,-6};

// THE AMP VALUES
unsigned char AMP1 = 0;
unsigned char AMP2 = 0;
unsigned char AMP3 = 0;
unsigned char AMP4 = 0;


// ======================================================
// SETUP STUFF
// ======================================================

void setup_intr()
{
  // setup timer1 to trigger at SAMPLE RATE
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  OCR1A = COMPARE_MATCH_VALUE;
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS10);
  TIMSK1 |= (1 << OCIE1A);
  sr_timer_ctr = 0;
}

void setup_pwm()
{
  pinMode(11, OUTPUT);
  pinMode(3, OUTPUT);
  // set Fast PWM mode
  TCCR2A = _BV(COM2A1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS20);
  PWM_OUTPUT_VALUE(127);
}

void setup() 
{
  Serial.begin(9600);
  cli(); // no interrupts 4 u
  setup_intr();
  setup_pwm();
  sei(); // renable interrupts
}

// ======================================================
// AUDIO GENERATION INTERRUPT
// ======================================================


unsigned int freq_period = 0;
unsigned int wave_table_index = 0;

// 32 bits
unsigned long phase_accumulator = 0;
unsigned long phase_incr = 1;
signed int out_v = 0;
unsigned int wave_max = 128;
ISR(TIMER1_COMPA_vect)
{
  phase_accumulator += phase_incr;

  // use the upper 7 bits to index into the wave tables
  wave_table_index = (phase_accumulator) >> 25;
  out_v = (WAV_TABLE4[wave_table_index]);

  PWM_OUTPUT_VALUE( out_v );
}

// ======================================================
// MAIN LOOP
// ======================================================

unsigned long phase_max = 0xFFFFFFFF;
unsigned long one_hz_period = phase_max / SAMPLE_RATE;
void loop() {
  current_freq = map(analogRead(A0), 0, 1023, 16, 523);
  phase_incr = (current_freq * one_hz_period);
}
