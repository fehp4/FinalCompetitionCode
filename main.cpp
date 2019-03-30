/* Include preprocessor directives */
//vscode
//now using git clone

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

#define TURN90 235
#define TURN45 115
#define TURN40 105
#define TURN55 144
#define TURN270 705
#define ONEINCH 40
#define DEFAULTPOWER 35
#define PCONST 0.5

//Declarations for encoders & motors
DigitalEncoder right_encoder(FEHIO::P0_0);
DigitalEncoder left_encoder(FEHIO::P0_1);
FEHMotor right_motor(FEHMotor::Motor2,9.0);
FEHMotor left_motor(FEHMotor::Motor3,9.0);
FEHServo servo(FEHServo::Servo7);
AnalogInputPin Cds(FEHIO::P3_7);

//return the absolute value
int abs_a(int a){
    if(a<0){
        return -a;
    } else {
        return a;
    }
}

//correct difference between encoders
void correct(void){

    int difference = abs_a(right_encoder.Counts() - left_encoder.Counts());
    if(right_encoder.Counts() > left_encoder.Counts()){
        left_encoder.ResetCounts();
        while(left_encoder.Counts() <= difference){
            left_motor.SetPercent(35);
        }

        left_motor.Stop();
    }

    else if(left_encoder.Counts() > right_encoder.Counts()){
        right_encoder.ResetCounts();
        while(right_encoder.Counts() <= difference){
            right_motor.SetPercent(35);
        }

        right_motor.Stop();
    }

    Sleep(1.0);
}

// Function to move forward
void move_forward(int percent, int counts){

    int old_value = 0;
    int tilt = 0;

    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(percent);

    //While the average of the left and right encoder is less than counts, keep running motors
    while(left_encoder.Counts() < counts && right_encoder.Counts() < counts){

        tilt = (right_encoder.Counts() - left_encoder.Counts());

        //write to file every 5 counts
        if(right_encoder.Counts() > old_value + 5){

            SD.Printf("R_encoder: %d\tL_encoder: %d\n", right_encoder.Counts(), left_encoder.Counts());
            SD.Printf("Tilt: %d\tPercent done: %d\n\n", tilt, right_encoder.Counts()*100/counts);
            old_value = right_encoder.Counts();
        }

        tilt = abs_a(tilt);

        //balance encoder counts by changing motor speed
        if(right_encoder.Counts() < left_encoder.Counts()){
            right_motor.SetPercent(percent + (tilt * PCONST));
            left_motor.SetPercent(percent);
        }
        else if (left_encoder.Counts() < right_encoder.Counts()){
            left_motor.SetPercent(percent + (tilt * PCONST));
            right_motor.SetPercent(percent);
        }
        else{
            right_motor.SetPercent(percent);
            left_motor.SetPercent(percent);
        }
    }

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();

    Sleep(0.25);

    correct();

}

// Function to move backward
void move_backward(int percent, int counts){

    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(percent);

    int old_value = 0;
    int tilt = (right_encoder.Counts() - left_encoder.Counts());

    //write to file every 5 counts
    if(right_encoder.Counts() > old_value + 5){

        SD.Printf("R_encoder: %d\tL_encoder: %d\n", right_encoder.Counts(), left_encoder.Counts());
        SD.Printf("Tilt: %d\tPercent done: %d\n\n", tilt, right_encoder.Counts()*100/counts);
        old_value = right_encoder.Counts();
    }

    //While the average of the left and right encoder is less than counts, keep running motors
    while(left_encoder.Counts() < counts && right_encoder.Counts() < counts){
        if(right_encoder.Counts() < left_encoder.Counts()){
            right_motor.SetPercent(percent - 0.25);
            left_motor.SetPercent(percent);
        }
        else if (left_encoder.Counts() < right_encoder.Counts()){
            left_motor.SetPercent(percent - 0.25);
            right_motor.SetPercent(percent);
        }
        else{
            right_motor.SetPercent(percent);
            left_motor.SetPercent(percent);
        }
    }

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
    Sleep(0.25);

}

//function where one motor must run faster than the other
void adjustMotor(int percent, int counts){
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(percent + 2);

    //While the average of the left and right encoder is less than counts, keep running motors
    while(left_encoder.Counts() < counts && right_encoder.Counts() < counts);

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();

    Sleep(1.0);
}

// Function to turn right
void turn_right(int percent, int counts){

    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();


    //Set both motors to desired percent
    //hint: set right motor backwards, left motor forwards

    right_motor.SetPercent(-percent);
    left_motor.SetPercent(percent);

    //While the average of the left and right encoder is less than counts,
    //keep running motors


    while(left_encoder.Counts() < counts) {

        right_motor.SetPercent(-percent);
        left_motor.SetPercent(percent);

    }

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

// Function to turn left
void turn_left(int percent, int counts){

    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent

    right_motor.SetPercent(percent);
    left_motor.SetPercent(-percent);

    //While the average of the left and right encoder is less than counts,
    //keep running motors

    while(right_encoder.Counts() < counts) {

        right_motor.SetPercent(percent);
        left_motor.SetPercent(-percent);
    }


    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

int check_heading(float heading) //using RPS
{

    SD.Printf("\n\nHEADING BEFORE: %f\n", RPS.Heading());
    LCD.WriteLine(RPS.Heading());

    if (RPS.Heading() < 0){
        return 2;
    }


    //NOTE: DO NOT PASS IN heading = 0
    while(RPS.Heading() < heading - 5 || RPS.Heading() > heading + 5){

        if(RPS.Heading() < heading - 5){
            turn_left(30, 12);

        }

        else if(RPS.Heading() > heading + 5){
            turn_right(30, 12);

        }

       // LCD.WriteLine(RPS.Heading());
        Sleep(.25);

    }

    SD.Printf("\n\nHEADING AFTER: %f\n", RPS.Heading());
    LCD.WriteLine(RPS.Heading());

    return 0;

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

//time is in seconds not milliseconds
void RunMotor(float sleep1, int percent)
{

    left_motor.SetPercent(percent);
    right_motor.SetPercent(percent);
    Sleep(sleep1);

    left_motor.Stop();
    right_motor.Stop();

    left_encoder.ResetCounts();
    right_encoder.ResetCounts();

    Sleep(1.0);
}

//function to sweep DDR light
float rotateDDR(int counts){

    float c = 1;
    int turn = 0;

    //run for i iterations
    for(int i = 0; i < 3; ++i){

        //rotates and saves the largest number
        while(turn < counts){
            turn_right(20, 5);

            if(Cds.Value() < c){
                c = Cds.Value();
                SD.Printf("CdS Value: %f\n", c);
            }

            Sleep(0.05);
            turn = turn + 5;
        }

        SD.Printf("\nNext Iteration. Min CdS Vale: %f\n\n", c);\
        //reset angle, move forward, and repeat movement
        //LCD.WriteLine("CdS Value: ");
       // LCD.Write(c);

        turn = 0;
        Sleep(1.0);
        turn_left(40, counts + 5);
        Sleep(1.0);

        if(c < 0.8){
            return c;
        }

        move_forward(10, ONEINCH);
    }
 }

//function to complete DDR task and stop before ramp
void DDR_task(){

    SD.Printf("\n\nEXITING START BLOCK:\n\n");
    servo.SetDegree(80);
    move_forward(35, 3.5*ONEINCH);
    turn_right(35, TURN45 );

    SD.Printf("\n\nGOING TO DDR LIGHT\n");
    move_forward(35, 10.5*ONEINCH);

    float c;

    SD.Printf("\n\nSWEEPING\n");
    c = rotateDDR(30);

    if(c < 0.25) {

        SD.Printf("\n\nDDR Task: Red light\n\n");
        LCD.WriteLine("DDR TASK: RED LIGHT");
        LCD.SetBackgroundColor(RED);
        LCD.Clear();

        SD.Printf("\nMoving Backwards\n\n");
        move_backward(-35, ONEINCH * 5);
        turn_left(35 , TURN90);
        SD.Printf("\nMoving Forwards\n\n");
        move_forward(35, ONEINCH * 3);
        turn_right(35, TURN90);
        SD.Printf("\nMoving Forwards\n\n");
        move_forward(35, ONEINCH * 6.0);
        turn_right(35, TURN90);

        SD.Printf("\nRuns into Button\n\n");
        move_forward(25, ONEINCH * 1.5);
        RunMotor(5.5, 25);

        move_backward(-35, ONEINCH * 2);
        turn_left(35, TURN90);


        RunMotor(3, 35);


    } else {

        SD.Printf("\n\nDDR Task: Blue light\n\n");
        LCD.WriteLine("DDR TASK: BLUE LIGHT");
        LCD.SetBackgroundColor(BLUE);
        LCD.Clear();

        move_backward(-35 , ONEINCH * 5);
        turn_left(35 , TURN90);
        move_forward(35, ONEINCH * 3);
        turn_right(35, TURN90);
        move_forward(35, ONEINCH * 13.5);
        turn_right(35, TURN90);

        RunMotor(7, 35);

        move_backward(-35, ONEINCH * 2);
        turn_left(35, TURN90);

        RunMotor(1, 35);

    }

    move_backward(-35 , ONEINCH * 1.5);
    RunMotor(2, 35);
    move_backward(-35 , ONEINCH * 1.5);
    turn_left(35, TURN90);
}

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
    while(count < 105){
        Sleep(50);
        servo.SetDegree(initialDegree + count);
        count++;
    }
    Sleep(2.0);
    servo.SetDegree(80);

}

void lever_coin()
{
    int i;

    move_forward(55 , 6 * ONEINCH);
    turn_left(35 , TURN45);
    move_forward(55 , 3.0 * ONEINCH);
    // servo arm hits lever
    move_servo();
    move_forward(50, ONEINCH*3.0);
    turn_left(35, TURN45);
    move_forward(50, ONEINCH * 11.0);

    i = check_heading(270);

    if(i != 0){
        LCD.WriteLine("RPS ERROR");
        Sleep(2.0);\
    }

    turn_right(35 , TURN90);
    // Runs into wall
    RunMotor(0.75 , 35);
    move_backward(-50, ONEINCH * 14);
    turn_left(35, TURN90);

    RunMotor(1.5,35);
    servo_coin();
}

void RunMotorFoosball(float sleep1, int percent)
{
    left_motor.SetPercent(percent);
    right_motor.SetPercent(percent);
    Sleep(sleep1);

    left_motor.Stop();
    right_motor.Stop();
    Sleep(1.0);

}

void foosball(){

    //starts after robot has moved back from foosball wall

    /*turn_left(25, TURN45);
    Sleep(0.25);
    move_forward(35 , ONEINCH * 1.75);
    Sleep(0.25);
    turn_left(25 , 130);*/

    Sleep(0.25);
    turn_left(25 , TURN90);

    move_forward(25 , ONEINCH * 2.5);
    Sleep(0.25);
    servo.SetDegree(2);
    Sleep(0.5);
    //RunMotor(0.5 , 45);
    move_forward(45, 4 * ONEINCH);
    Sleep(0.25);
    servo.SetDegree(70);

}


void ramp(){

    int i;

    SD.Printf("\nRAMP:\n\n");
    //go to top of ramp
    move_forward(55, 41 * ONEINCH);
    Sleep(0.25);
    i = check_heading(90);

    if(i != 0){
        LCD.WriteLine("RPS ERROR");
        Sleep(2.0);\
        turn_right(25, 25);
    }

    SD.Printf("\nGoing Down ramp with adjusted motor\n\n");
    move_forward(35, 11 * ONEINCH);

    Sleep(0.25);
    RunMotor(2 , 35);
    Sleep(0.25);


    SD.Printf("\nMoving Backwards\n\n");
    move_backward(-35 , ONEINCH * 1.5);
    Sleep(0.25);

}

void final_button(){

    move_backward(-35 , ONEINCH * 4);
    turn_right(35 , TURN90);
    move_forward(55 , ONEINCH * 10.5);
    turn_left(35 , TURN90);
    // down the ramp
    move_forward(55 , ONEINCH * 28);
    turn_left(25 , TURN45);
    move_forward(55 , ONEINCH * 2);
    turn_right(25 , TURN45);
    RunMotor(1.0 , 25);
}




int main(void){

    RPS.InitializeTouchMenu();

    SD.OpenLog();

    SD.Printf("VERSION: %s\nBATTERY VOLTAGE: %f\n\n\n","1.0.0", Battery.Voltage());

    //move_backward(-25, ONEINCH * 4);

    //Servo motor max and min values
    servo.SetMin(514);
    servo.SetMax(3500);

    //Initialize the screen
    LCD.Clear(BLACK);
    LCD.SetFontColor(WHITE);

    while(Cds.Value() >= 1);

    float a = Cds.Value();
    Sleep(0.5);

    SD.Printf("Start CdS Value: %f\n\n", a);

    DDR_task();
    ramp();
    foosball();

    //after foosball task
    lever_coin();
    final_button();

}

