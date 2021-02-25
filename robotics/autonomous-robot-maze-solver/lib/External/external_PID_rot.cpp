/**********************************************************************************************
 * Arduino PID Library - Version 1.2.1
 * by Brett Beauregard <br3ttb@gmail.com> brettbeauregard.com
 *
 * This Library is licensed under the MIT License
 **********************************************************************************************/

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include <external_PID_rot.h>

/*Constructor (...)*********************************************************
 *    The parameters specified here are those for for which we can't set up
 *    reliable defaults, so we need to have the user set them.
 ***************************************************************************/
PID_Rot::PID_Rot(double* Input, double* Output, double* Setpoint,
        double Kp, double Ki, double Kd, int POn, int ControllerDirection)
{
    myOutput = Output;
    myInput = Input;
    mySetpoint = Setpoint;
    inAuto = false;

    PID_Rot::SetOutputLimits(0, 255);				//default output limit corresponds to
												//the arduino pwm limits

    SampleTime = 100;							//default Controller Sample Time is 0.1 seconds

    PID_Rot::SetControllerDirection(ControllerDirection);
    PID_Rot::SetTunings(Kp, Ki, Kd, POn);

    lastTime = millis()-SampleTime;
}

/*Constructor (...)*********************************************************
 *    To allow backwards compatability for v1.1, or for people that just want
 *    to use Proportional on Error without explicitly saying so
 ***************************************************************************/

PID_Rot::PID_Rot(double* Input, double* Output, double* Setpoint,
        double Kp, double Ki, double Kd, int ControllerDirection)
    :PID_Rot::PID_Rot(Input, Output, Setpoint, Kp, Ki, Kd, P_ON_E, ControllerDirection)
{

}


/* Compute() **********************************************************************
 *     This, as they say, is where the magic happens.  this function should be called
 *   every time "void loop()" executes.  the function will decide for itself whether a new
 *   pid Output needs to be computed.  returns true when the output is computed,
 *   false when nothing has been done.
 **********************************************************************************/
bool PID_Rot::Compute()
{
   if(!inAuto) return false;
   unsigned long now = millis();
   unsigned long timeChange = (now - lastTime);
   if(timeChange>=SampleTime)
   {
        /*Compute all the working error variables*/
        double input = *myInput;
        //input += 180.0;
        double set = *mySetpoint;
        //set += 180.0;
        
        //double error = diff(input, set);
        double error = *mySetpoint - input;
        if (error < -180) error += 360;
        else if (error > 180) error -= 360;

        Serial.println("IN PID COMPUTE()");
        Serial.print("Error = "); Serial.println(error);
        Serial.print("Kp = "); Serial.print((double)kp); Serial.print("\tKi = ");
        Serial.print((double)ki); Serial.print("\tKd = "); Serial.println(kd);

        double dInput = (input - lastInput);
        outputSum += (ki * error);

        Serial.print("ki * error = "); Serial.println(ki * error);

        /*Add Proportional on Measurement, if P_ON_M is specified*/
        if(!pOnE) outputSum -= kp * dInput;
      
        Serial.print("outputSum -= kp * dInput; kp * dInput = "); Serial.println(kp*dInput);

        if(outputSum > 4000) outputSum= 4000;
        else if(outputSum < -4000) outputSum= -4000;

        Serial.print("clamped outputSum = "); Serial.println(outputSum);

        /*Add Proportional on Error, if P_ON_E is specified*/
	    double output;
        if(pOnE) output = kp * error;
        else output = 0;
    
        Serial.print("kp * error = "); Serial.println(kp * error);

        /*Compute Rest of PID Output*/
        output += outputSum - kd * dInput;

        Serial.print("outSum = "); Serial.println(outputSum);
        Serial.print("kd * dInput = "); Serial.println(kd * dInput);
        Serial.print("output = "); Serial.println(output);

        //if (output < 0) output *= -1;

	    if(output > outMax) output = outMax;
        else if(output < -outMax) output = -outMax;
        *myOutput = output;

        /*Remember some variables for next time*/
        lastInput = input;
        lastTime = now;
	    return true;
   }
   else return false;
}

/* SetTunings(...)*************************************************************
 * This function allows the controller's dynamic performance to be adjusted.
 * it's called automatically from the constructor, but tunings can also
 * be adjusted on the fly during normal operation
 ******************************************************************************/
void PID_Rot::SetTunings(double Kp, double Ki, double Kd, int POn)
{
   if (Kp<0 || Ki<0 || Kd<0) return;

   pOn = POn;
   pOnE = POn == P_ON_E;

   dispKp = Kp; dispKi = Ki; dispKd = Kd;

   double SampleTimeInSec = ((double)SampleTime)/1000;
   kp = Kp;
   ki = Ki * SampleTimeInSec;
   kd = Kd / SampleTimeInSec;

  if(controllerDirection ==REVERSE)
   {
      kp = (0 - kp);
      ki = (0 - ki);
      kd = (0 - kd);
   }
}

/* SetTunings(...)*************************************************************
 * Set Tunings using the last-rembered POn setting
 ******************************************************************************/
void PID_Rot::SetTunings(double Kp, double Ki, double Kd){
    SetTunings(Kp, Ki, Kd, pOn); 
}

/* SetSampleTime(...) *********************************************************
 * sets the period, in Milliseconds, at which the calculation is performed
 ******************************************************************************/
void PID_Rot::SetSampleTime(int NewSampleTime)
{
   if (NewSampleTime > 0)
   {
      double ratio  = (double)NewSampleTime
                      / (double)SampleTime;
      ki *= ratio;
      kd /= ratio;
      SampleTime = (unsigned long)NewSampleTime;
   }
}

/* SetOutputLimits(...)****************************************************
 *     This function will be used far more often than SetInputLimits.  while
 *  the input to the controller will generally be in the 0-1023 range (which is
 *  the default already,)  the output will be a little different.  maybe they'll
 *  be doing a time window and will need 0-8000 or something.  or maybe they'll
 *  want to clamp it from 0-125.  who knows.  at any rate, that can all be done
 *  here.
 **************************************************************************/
void PID_Rot::SetOutputLimits(double Min, double Max)
{
   if(Min >= Max) return;
   outMin = Min;
   outMax = Max;

   if(inAuto)
   {
	   if(*myOutput > outMax) *myOutput = outMax;
	   else if(*myOutput < outMin) *myOutput = outMin;

	   if(outputSum > outMax) outputSum= outMax;
	   else if(outputSum < outMin) outputSum= outMin;
   }
}

// // Get the difference between 2 degree values on a cyclic scale from 0 -> 360, can be rewritten better later
double PID_Rot::diff (double val, double ref) {
    if (val >= ref) {
        if ( (ref + (360 - val)) > (val - ref) ) {
            return (val - ref);
        } else {
            return (ref + (360 - val));
        }
    } else {
        if ( (val + (360 - ref)) > (ref - val) ) {
            return (ref - val);
        } else {
            return (val + (360 - ref));
        }
    }
}

/* SetMode(...)****************************************************************
 * Allows the controller Mode to be set to manual (0) or Automatic (non-zero)
 * when the transition from manual to auto occurs, the controller is
 * automatically initialized
 ******************************************************************************/
void PID_Rot::SetMode(int Mode)
{
    bool newAuto = (Mode == AUTOMATIC);
    if(newAuto && !inAuto)
    {  /*we just went from manual to auto*/
        PID_Rot::Initialize();
    }
    inAuto = newAuto;
}

/* Initialize()****************************************************************
 *	does all the things that need to happen to ensure a bumpless transfer
 *  from manual to automatic mode.
 ******************************************************************************/
void PID_Rot::Initialize()
{
   outputSum = *myOutput;
   lastInput = *myInput;
   if(outputSum > outMax) outputSum = outMax;
   else if(outputSum < outMin) outputSum = outMin;
}

/* SetControllerDirection(...)*************************************************
 * The PID will either be connected to a DIRECT acting process (+Output leads
 * to +Input) or a REVERSE acting process(+Output leads to -Input.)  we need to
 * know which one, because otherwise we may increase the output when we should
 * be decreasing.  This is called from the constructor.
 ******************************************************************************/
void PID_Rot::SetControllerDirection(int Direction)
{
   if(inAuto && Direction !=controllerDirection)
   {
	    kp = (0 - kp);
      ki = (0 - ki);
      kd = (0 - kd);
   }
   controllerDirection = Direction;
}

/* Status Funcions*************************************************************
 * Just because you set the Kp=-1 doesn't mean it actually happened.  these
 * functions query the internal state of the PID.  they're here for display
 * purposes.  this are the functions the PID Front-end uses for example
 ******************************************************************************/
double PID_Rot::GetKp(){ return  dispKp; }
double PID_Rot::GetKi(){ return  dispKi;}
double PID_Rot::GetKd(){ return  dispKd;}
int PID_Rot::GetMode(){ return  inAuto ? AUTOMATIC : MANUAL;}
int PID_Rot::GetDirection(){ return controllerDirection;}

