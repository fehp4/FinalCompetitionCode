/* Include preprocessor directives */
#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHServo.h>
#include <FEHAccel.h>
#include <FEHBattery.h>
#include <FEHBuzzer.h>
#include <FEHRPS.h>
#include <FEHSD.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

// Constants
#define TURN90 235
#define TURN45 117
#define TURN50 130
#define ONEINCH 40
#define DEFAULTPOWER 25

//Declarations for encoders & motors
DigitalEncoder right_encoder(FEHIO::P0_0);
DigitalEncoder left_encoder(FEHIO::P0_1);
FEHMotor right_motor(FEHMotor::Motor2,9.0);
FEHMotor left_motor(FEHMotor::Motor3,9.0);
FEHServo servo(FEHServo::Servo7);
AnalogInputPin Cds(FEHIO::P3_7);

// Function to return absolute value
int abs(int a)
{
    if(a<0)
        return -a;
    else
        return a;
}

// Function to check encoder counts and correct if required
void correct(void){

    int difference = abs(right_encoder.Counts() - left_encoder.Counts());
    if(right_encoder.Counts() > left_encoder.Counts())
    {
        left_encoder.ResetCounts();
        while(left_encoder.Counts() <= difference)
        {
            left_motor.SetPercent(25);
        }
        left_motor.Stop();
    }
    else if(left_encoder.Counts() > right_encoder.Counts())
    {
        right_encoder.ResetCounts();
        while(right_encoder.Counts() <= difference)
        {
            right_motor.SetPercent(25);
        }
        right_motor.Stop();
    }
    else{
        LCD.WriteLine("left and right same counts.");
    }


    Sleep(1.0);
}


// Function to move forward or backward
void move_forward_backward(int percent, int counts) //using encoders
{

    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(percent + 1);

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while(left_encoder.Counts() < counts && right_encoder.Counts() < counts);

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();

    Sleep(1.0);


}

void adjustMotor(int percent, int counts)
{

    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(percent + 1);

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while(left_encoder.Counts() < counts && right_encoder.Counts() < counts);



    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();

    Sleep(1.0);
}

// Function to turn right
void turn_right(int percent, int counts) //using encoders
{

    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();


    //Set both motors to desired percent
    right_motor.SetPercent(-percent);
    left_motor.SetPercent(percent + 1);

    //While the average of the left and right encoder is less than counts,
    //keep running motors

    while(left_encoder.Counts() < counts)
    {

        right_motor.SetPercent(-percent);
        left_motor.SetPercent(percent + 1);

    }

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}


// Function to turn left
void turn_left(int percent, int counts) //using encoders
{

    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent

    right_motor.SetPercent(percent);
    left_motor.SetPercent(-percent);

    //While the average of the left and right encoder is less than counts,
    //keep running motors

    while(right_encoder.Counts() < counts)
    {

        right_motor.SetPercent(percent);
        left_motor.SetPercent(-percent);


    }

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}


// Function to move servo arm
void move_servo()
{

    servo.SetDegree(80);
    Sleep(1.0);
    servo.SetDegree(0);
    Sleep(1000);
    servo.SetDegree(80);

}


// Function to return optosensor value
float optosensor(AnalogInputPin CdS)
{
    float value = CdS.Value();
    return value;

}

// Function for the DDR task
void DDR()
{
    // Checking if light is red
    if(Cds.Value()  > 0.1  && Cds.Value() < 0.2)
    {
        LCD.WriteLine(" DDR Task: Red light ");
        move_forward_backward(-25, ONEINCH * 5);
        turn_left(25 , TURN90);
        move_forward_backward(25, ONEINCH * 3);
        correct();
        turn_right(25, TURN90);
        move_forward_backward(25, ONEINCH * 5.5);

        turn_right(25, TURN90);

        left_motor.SetPercent(25);
        right_motor.SetPercent(25);
        Sleep(9.0);
        left_motor.Stop();
        right_motor.Stop();


        move_forward_backward(-25, ONEINCH);
        turn_left(25, TURN90);
        move_forward_backward(25, ONEINCH * 9);
        left_motor.SetPercent(25);
        right_motor.SetPercent(25);
        Sleep(3.0);
        left_motor.Stop();
        right_motor.Stop();
        move_forward_backward(-25 , ONEINCH);
        turn_left(25, TURN90);




    }
    // Checking if light is blue
    else if(Cds.Value() >  0.25  && Cds.Value() < 0.35)
    {
        LCD.WriteLine(" DDR Task: Blue light ");
        move_forward_backward(-25, ONEINCH * 5);
        turn_left(25 , TURN90);
        move_forward_backward(25, ONEINCH * 3);
        correct();
        turn_right(25, TURN90);

        move_forward_backward(25, ONEINCH * 13.5);
        //correct();
        turn_right(25, TURN90);
        left_motor.SetPercent(25);
        right_motor.SetPercent(25);

        Sleep(7.0);
        left_motor.Stop();
        right_motor.Stop();
        move_forward_backward(-25, ONEINCH);
        turn_left(25, TURN90);

        left_motor.SetPercent(25);
        right_motor.SetPercent(25);
        Sleep(5.0);
        left_motor.Stop();
        right_motor.Stop();
        move_forward_backward(-25 , ONEINCH);
        turn_left(25, TURN90);

    }
    else
    {}

}

// Function to move servo arm slowly so it hits the dowel rod
void servo_coin()
{
    int initialDegree = 80;
    int count = 1;
    while(count < 10){
        Sleep(50);
        servo.SetDegree(initialDegree + count);
        count++;
    }
    Sleep(3.0);
    initialDegree = initialDegree + count - 1;
    count = 1;
    // Servo arm moves slowly until angle is 105 degrees
    while(count < 105){
        Sleep(50);
        servo.SetDegree(initialDegree + count);
        count++;
    }
    Sleep(2.0);
    servo.SetDegree(80);

}

// Function for lever and coin task
void lever_coin()
{
    move_forward_backward(25, 17.5*ONEINCH);
    //turn 40 degrees
    turn_left(25, 105);
    move_forward_backward(25, ONEINCH * 5.5);

    move_servo();
    move_forward_backward(25, ONEINCH*3);

    //turn 45 degrees
    turn_left(25, 130);
    move_forward_backward(25, 13 *ONEINCH);
    turn_right(25 , TURN90);
    right_motor.SetPercent(25);
    left_motor.SetPercent(25);
    Sleep(4.0);
    right_motor.Stop();
    left_motor.Stop();

    //turn_left(25, TURN90);
    move_forward_backward(-25, ONEINCH * 15 - 10);


    turn_left(25, TURN90);

    right_motor.SetPercent(30);
    left_motor.SetPercent(35);
    Sleep(2.0);
    right_motor.Stop();
    left_motor.Stop();
    // calling function to move servo arm and hit dowel rod
    servo_coin();
}

//time is in seconds not milliseconds
void RunMotor(float sleep1, int percent)
{

    left_motor.SetPercent(percent);
    right_motor.SetPercent(percent);
    Sleep(sleep1);

    left_motor.Stop();
    right_motor.Stop();
    Sleep(1.0);
}

// Function for
void RunMotorFoosball(float sleep1, int percent)
{
    left_motor.SetPercent(percent);
    right_motor.SetPercent(percent);
    Sleep(sleep1);

    left_motor.Stop();
    right_motor.Stop();
    Sleep(1.0);

}


int main(void)
{

    // Servo motor max and min values
    servo.SetMin(514);
    servo.SetMax(2500);

    //Initialize the screen
    LCD.Clear(BLACK);
    LCD.SetFontColor(WHITE);

    // Checking for start light
    while(Cds.Value() >= 1);

    servo.SetDegree(90);
    move_forward_backward(25, 6*ONEINCH);
    turn_right(25, TURN45 );
    adjustMotor(25, 18*ONEINCH+30);
    // bumping into wall to straighten robot
    left_motor.SetPercent(25);

    right_motor.SetPercent(25);
    Sleep(1.0);
    left_motor.Stop();
    right_motor.Stop();
    move_forward_backward(-25 , ONEINCH);
    turn_left(25, TURN90);
    //go to top of ramp
    adjustMotor(55, 25*ONEINCH);
    correct();
    move_forward_backward(55, 28.5 * ONEINCH);
    // Bumping into foosball wall
    RunMotor(1.0, 35);
    Sleep(1.0);
    RunMotor(0.3 , -35);
    Sleep(1.0);
    turn_left(25, TURN45);

    Sleep(1.0);
    RunMotor(0.4 , 35);
    Sleep(1.0);
    turn_left(25 , TURN45);
    move_forward_backward(25 , ONEINCH);
    Sleep(1.0);
    // Setting servo arm to 5 degrees
    servo.SetDegree(5);
    Sleep(1.0);
    // Dragging foosball counters
    RunMotor(2.0, 35);
    Sleep(1.0);

    // Setting servo arm to 90 degrees
    servo.SetDegree(90);
    Sleep(1.0);
    // Moving backward
    move_forward_backward(-35 , ONEINCH * 2.5);

    Sleep(1.0);
    servo.SetDegree(5);
    Sleep(1.0);
    RunMotor(1.0, 35);
    Sleep(1.0);
    servo.SetDegree(90);


    // after foosball task
    move_forward_backward(25 , 4);
    turn_left(25 , TURN45);
    move_forward_backward(25 , 6);
    turn_left(25 , TURN45);
    RunMotor(7.0 , 40);
    return 0;
}
