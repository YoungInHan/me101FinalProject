//written by group 68
#include "PC_FileIO.c"

//function prototypes
int moveUntilSense ();
int findDirection();
void avoidObstacle ();
void dataCollection (float* position, int & count, int encoder_value);
void outputFile (float * coordinates);

task main()
{

	//configure sensors
	SensorType[S1] = sensorEV3_Ultrasonic;
	wait1Msec(50);
	SensorType[S2] = sensorEV3_Ultrasonic;
	wait1Msec(50);
	SensorType[S3] = sensorEV3_Ultrasonic;
	wait1Msec(50);

	SensorType[S4] = sensorEV3_Gyro;
	SensorMode[S4] = modeEV3Gyro_RateAndAngle;
	wait1Msec(50);

	//wait for user to press enter button
	while(!getButtonPress(buttonEnter))
	{}
	while(getButtonPress(buttonAny))
	{}


	//turn fan on maximum power
	motor[motorB] = -100;

	//declare variables
	float position[30];
	int encoder_value = 0;

	//set all values in array to zero
	for (int i = 0; i < 30; i++)
		position[i] = 0;

	wait1Msec(250);

	//run movement program a total of 15 times
	for (int count = 0; count < 15; count++)
	{
		encoder_value = moveUntilSense();
		wait1Msec(150);
		dataCollection(position,count,encoder_value);
		avoidObstacle();
		wait1Msec(150);
	}

	outputFile(position);

	//turn fan off
	motor[motorB] = 0;

	//press enter button to exit program
	while(!getButtonPress(buttonEnter))
	{}
	while(getButtonPress(buttonAny))
	{}

}

//moves the robot in the forward until ultrasonic
//sensor's value is a certain distance
int moveUntilSense ()
{
	const int DIST = 10;

	//reset nMotorEncoder
	int encoder_value = 0;
	nMotorEncoder[motorA] = 0;

	motor[motorA] = motor[motorD] = 25;

	while(SensorValue(S1) > DIST && SensorValue(S2) > DIST
		&& SensorValue(S3) > DIST)
	{}

	motor[motorA] = motor[motorD] = 0;

	//return encoder value to main
	encoder_value = nMotorEncoder[motorA];
	return encoder_value;
}

//turns the robot according to a recieved direction value
void avoidObstacle ()
{

	int angle = getGyroDegrees(S4);
	int direction = findDirection ();

	//find the equivalent direction according to the current gyro angle
	int eq_direction = angle + direction;
	if (eq_direction > angle+180)
		eq_direction = eq_direction-360;

	if (angle > eq_direction)
	{
		//CCW turn
		motor[motorA] = 25;
		motor[motorD] = -25;
	}
	else
	{
		//CW turn
		motor[motorA] = -25;
		motor[motorD] = 25;
	}

	//keeps turning until the change in angle meets the direction desired
	int delta_angle = SensorValue(S4) - angle;
	while (abs(delta_angle) < abs(direction))
	{
		delta_angle = SensorValue(S4) - angle;
	}

	motor[motorA] = motor[motorD] = 0;
}

//find direction to move according to sensor values
int findDirection()

{
	//initialize the sensor weights as very high numbers
	float weightSens1 = 9999, weightSens2 = 9999, weightSens3 = 9999;

	//sensor weights are inversely proportial to the sensor values
	if (SensorValue(S1) != 0)
		weightSens1 = 10.0/SensorValue(S1);
	if (SensorValue(S2) != 0)
		weightSens2 = 10.0/SensorValue(S2);
	if (SensorValue(S3) != 0)
		weightSens3 = 10.0/SensorValue(S3);

	// The direction it is facing is a weighted average
	// Weight value depends on the angle the sensor is directed from the front
	float weight = weightSens1 + weightSens2 + weightSens3;
	int hit_direction = (weightSens2*-60 + weightSens3*60)/weight;
	int direction = 0;

	if (hit_direction < 0)
	{
		direction += 30;
		if (hit_direction > -30)
		{
			direction += 30;
		}
		if (hit_direction > -15)
		{
			direction += 30;
		}
	}
	else
	{
		direction -= 30;
		if (hit_direction < 30)
		{
			direction -= 30;
		}
		if (hit_direction < 15)
		{
			direction -= 30;
		}
	}

	//add a random element to motion to prevent the robot from being stuck
	direction = direction - 15 + random(30);

	return direction;
}

//calculate and store the (x,y) position of the robot when stopped
void dataCollection (float* position, int &count, int encoder_value)
{
	const float RADIUS = 2.75;
	int angle = getGyroDegrees(S4);

	//make angle into positive reference angle
	angle = -1*angle;
	while (angle < 0)
	{
		angle += 360;
	}
	angle %= 360;

	//calculate distance travelled
	float distance = encoder_value*RADIUS*PI/180.0;

	//get previous x & y coordinates from array
	float pre_x = position[2*count];
	float pre_y = position[2*count+1];

	//calculate the change in x & y coordinates
	float delta_x = distance*cosDegrees((float)angle);
	float delta_y = distance*sinDegrees((float)angle);

	//calculate current x & y coordinates
	float x = pre_x + delta_x;
	float y = pre_y + delta_y;

	//store x & y coordinates to the array
	position[2*count+2] = x;
	position[2*count+3] = y;
}

//output coordinate array to a file
void outputFile (float * coordinates)
{

	TFileHandle fout;

	word fileSize = 1000;

	bool fileOkay = openWritePC(fout, "Data.txt", fileSize);

	if(!fileOkay)
	{
		displayString (1, "Error");

		wait1Msec(5000);
	}

	else
	{
		writeEndlPC (fout);

		for (int line = 0; line < 30; line+=2)
		{
			int xCoord = (int)coordinates[line];
			int yCoord = (int)coordinates[line+1];
			writeFloatPC (fout, "%d ", xCoord);
			writeFloatPC (fout, "%d", yCoord);
			writeEndlPC (fout);
		}
		closeFilePC (fout);
	}
}
