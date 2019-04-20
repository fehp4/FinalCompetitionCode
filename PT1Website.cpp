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
#define ONEINCH 40
#define DEFAULTPOWER 25

//Declarations for encoders, motors and servo
DigitalEncoder right_encoder(FEHIO::P0_0);
DigitalEncoder left_encoder(FEHIO::P0_1);
FEHMotor right_motor(FEHMotor::Motor0,9.0);
FEHMotor left_motor(FEHMotor::Motor1,9.0);
FEHServo servo(FEHServo::Servo0);
AnalogInputPin Cds(FEHIO::P3_7);


// Function to return absolute value
int abs(int a)
{
    if(a<0)
        return -a;
    else
        return a;
}

// correct function to check encoder counts and correct if necessary
void correct(void){

    int difference = abs(right_encoder.Counts() - left_encoder.Counts());
    if(right_encoder.Counts() > left_encoder.Counts())
    {
        left_encoder.ResetCounts();
        while(left_encoder.Counts() < difference)
        {
            left_motor.SetPercent(25);
        }
    }
    else if(left_encoder.Counts() > right_encoder.Counts())
    {
        right_encoder.ResetCounts();
        while(right_encoder.Counts() < difference)
        {
            right_motor.SetPercent(25);
        }
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
    left_motor.SetPercent(percent);

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
    left_motor.SetPercent(percent);

    //While the average of the left and right encoder is less than counts,
    //keep running motors


    while(left_encoder.Counts() < counts)
    {

        right_motor.SetPercent(-percent);
        left_motor.SetPercent(percent);

    }

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}


// Function to turn left
void turn_left(int percent, int counts) //using encoders
{

    //Reset encoder counts
    //LCD.WriteLine("Turning left");
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


// Function to move servo arm for the lever task
void move_servo()
{

    servo.SetDegree(90);
    Sleep(1.0);
    servo.SetDegree(0);
    Sleep(1000);
    servo.SetDegree(90);

}



// Function to return optosensor value
float optosensor(AnalogInputPin CdS)
{
    float value = CdS.Value();
    return value;

}

// Function to reset both the encoder counts
void encoder_counts(void)
{
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

}


// main function
int main(void)
{


    // Servo motor max and min values
    servo.SetMin(514);
    servo.SetMax(2500);



    float x, y; //for touch screen

    //Initialize the screen
    LCD.Clear(BLACK);
    LCD.SetFontColor(WHITE);

    // Checking for start light
    while(Cds.Value() >= 1);

    move_forward_backward(25, 6*ONEINCH);
    correct();
    servo.SetDegree(110);

    turn_right(25, TURN45 );
    move_forward_backward(25, 18*ONEINCH+20);
    correct();

    turn_left(25, TURN90);

    // Moving up the ramp at motor speed 55
    move_forward_backward(55, 25*ONEINCH);
    correct();

    move_forward_backward(25, 29*ONEINCH);
    correct();

    turn_left(25, TURN90);
    move_forward_backward(25, 16*ONEINCH+20);
    correct();

    turn_left(25, TURN45);
    move_forward_backward(25, ONEINCH*3);
    correct();

    // Hitting the lever
    move_servo();
    move_forward_backward(25, ONEINCH*4+20);
    correct();



    turn_left(25, TURN45);
    move_forward_backward(25, 12*ONEINCH);
    correct();
    // Going down the ramp at motor speed 45
    right_motor.SetPercent(45);
    left_motor.SetPercent(45);
    Sleep(5.0);
    left_motor.Stop();
    right_motor.Stop();

    return 0;
}

