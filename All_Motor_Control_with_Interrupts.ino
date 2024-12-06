//INTERRUPT TESTER
/* How Timer Interrupts work (I think)
The Arduino has a total of 3 different clocks in hardware. 
Timer 1 
Timer 2 
Timer 3
These timers have internal oscillators that oscillate at very high frequencies. 
The frequencies of these clocks are usually divided by a prescalar number that will divide down the timers signal. 
To set a desired frequency (like a frequency we want the timer to perform an interrupt), then we have to use a compare match register. 
These are essentially a number of counts that the timer will count to before it resets itself, every time the counter resets the interrupt is generated, causing the interrupt to activate at a predetermined frequency. 

By varying the compare match value on the output compare match register OCR1A, we can vary the frequency that the interrupt activates, and thus change the speed of the stepper motor which is being pulsed by it. 
*/

#define step_pin 2
#define dir_pin 3


//Defining Macros

#define TIMER_INTERRUPTS_ON     TIMSK1 |= (1 << OCIE1A);
#define TIMER_INTERRUPTS_OFF    TIMSK1 &= ~(1 << OCIE1A);
int c0;

void setup() {
  Serial.begin(9600);
  pinMode(step_pin, OUTPUT);
  pinMode(dir_pin, OUTPUT);
  
  noInterrupts(); //Do not do an interrupt yet cause we are doing some math.
  TCCR1A = 0; //Reset register A 
  TCCR1B = 0; //Reset register B, this is the register we are using
  OCR1A = 1000; //compare match register for desired interrupt frequency
  TCCR1B |= (1 << WGM12); //Set up CTC mode. Will reset TCCR1A when compare match is reached
  TCCR1B |= ((1 << CS11) | (1 << CS10)); //Set prescalar for port B register
  TCNT1 = 0; //reset timer 1

  c0 = 1600; //initial compare match value (ticks)
  interrupts();
}

//Define the volatile variables (ones we are gonna use in the ISR())
volatile int n = 0;
volatile int speed_Des = 500; //desired speed from ramp up
volatile int d; //Delay between steps. This will vary as a function of time. 
volatile int steps_taken = 0; //total steps taken
volatile int steps_commanded;
volatile int step_pos = 0; //sets our zero origin based on where the motor is at during the beginning of the program
volatile int steps_up = 0; //steps taken during ramp up phase
volatile bool movementDone = false; //check if the commanded steps have been completed
volatile int dir;


ISR(TIMER1_COMPA_vect){ //compare match mode
  if (steps_taken < steps_commanded) {
    digitalWrite(step_pin, HIGH);
    digitalWrite(step_pin, LOW);
    steps_taken++; //increase steps taken
    step_pos += dir;
  }

  else {
    volatile bool movementDone = true; 
    TIMER_INTERRUPTS_OFF;
  }

  if (steps_up == 0) { //are we in the ramp up phase?
    n++; //step up
    steps_up++;
    d = d - (2*d)/((4*n)+1); //calculate new delay
    
    //CONDITIONALS FOR ACCEL PROFILE
    if (d <= speed_Des) { //have we reached the desired speed?
      d = speed_Des; //cap speed at desired speed (accel profile is trapezoidal)
      steps_up = steps_taken; //steps up will not increase anymore
    }
    if (steps_up >= steps_commanded / 2) {
      steps_up = steps_taken;
    }
  }
  else if (steps_commanded >= steps_taken-steps_up) {
      n--; //step down
      d = d/(1-(2/(4*n+1))); // calculate delay in ramp down (solve for c_n-1)
  }
  OCR1A = d; 
}

void move(long steps) {
  digitalWrite(dir_pin, steps<0 ? HIGH : LOW); //set direction based on step pin sign
  dir = steps > 0 ? 1 : -1; //dir variable will either add or subtract steps depending on direction
  steps_commanded = abs(steps); //total steps regardless of sign
  d = c0; //initial delay variable
  OCR1A = d; //set timer register to the initial delay as a compare match value.
  steps_taken = 0; 
  movementDone = false;
  steps_up = 0;
  n = 0; //have to reset all the variables to their default numbers since the move function can be called multiple times

  TIMER_INTERRUPTS_ON; //activate interrupts
}

void moveTo(long pos, bool wait = true) {
  move(pos-step_pos);
  while ( wait && !movementDone );
}

void loop() {
  // put your main code here, to run repeatedly:
  //REPEAT SEQUENCE - TESTS IF MOTOR MOVES SYMMETRICALLY
  //Sequence 1
  moveTo(1600);
  moveTo(-1600);
  moveTo(0);
  /*
  //Sequence 2
  moveTo(200);
  moveTo(400);
  moveTo(600);
  moveTo(800);
  //Sequence 3
  moveTo(400);
  moveTo(600);
  moveTo(200);
  moveTo(400);
  moveTo(0);
  //Last sequences seeing what different speeds look like
  speed_Des = 1000;
  moveTo(200);
  moveTo(400);
  speed_Des = 800;
  moveTo(600);
  moveTo(800);
  speed_Des = 200; 
  moveTo(0);
  */
}
