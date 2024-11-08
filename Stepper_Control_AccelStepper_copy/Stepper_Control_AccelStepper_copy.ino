#include <AccelStepper.h>

//Define the step/direction pins
#define step1 2
#define dir1 3

#define step2 4 
#define dir2 5
/*
#define step3 6
#define dir3 7
//add last three when the MEGA arrives

#define step4 8
#define dir4 9

#define step5 10
#define dir5 11

#define step6 12
#define dir6 13
*/

AccelStepper J1(1, step1, dir1);
AccelStepper J2(1, step2, dir2);
//AccelStepper J3(1, step3, dir3);

void setup() {
  //You can vary the move-to function to change the motor position

  J1.setMaxSpeed(300); //15 steps per second rounded up a little ~ 4 rev/min
  J1.setAcceleration(100);
  J1.moveTo(200); // 200 steps ~ full revolution(full stepping) / half a revolution (half stepping)
  
  J2.setMaxSpeed(200); //15 steps per second rounded up a little ~ 4 rev/min
  J2.setAcceleration(100);
  J2.moveTo(-400); // 400 steps ~ full revolution(full stepping) / half a revolution (half stepping)
  /*
  J3.setMaxSpeed(200);
  J3.setAcceleration(100);
  J3.moveTo(600);
  */
}

void loop() {
  // put your main code here, to run repeatedly:
  //we use run here since it is not a blocking function. We may see later that run has some limitations (slow and velocity profile is not very linear)
  J1.run();
  //J2.run();
  //J3.run();

  //we'll have one motor moving continuously while the other motors move to a specific position and stop there. 
  if (J1.distanceToGo() == 0) {
    J1.moveTo(-J1.currentPosition());
  }
}
