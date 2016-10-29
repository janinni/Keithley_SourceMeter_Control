/*

SourceMeter.cpp


Created by Janine Müller on 05.10.2016

*/

#include <gpib/ib.h>
#include <gpib/gpib_user.h>
#include <string>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <fstream>
#include "time.h"
#include <sstream>

#include "SourceMeter.h"
#include "../gpib/gpib.h"
#include "../LogFileDev/LogDev.h"

using namespace std;

//---------------------SourceMeter class---------------------//

// Constructor
SourceMeter::SourceMeter() : 
	_outputStatusA(0), // Output status of Output A
	_outputStatusB(0), // Output status of Output B
	_actualVoltageA(0.), // actual source voltage of Output A
	_actualVoltageB(0.), // actual source voltage of Output B
	_actualCurrentA(0), // actual source current of Output A
	_actualCurrentB(0), // actual source current of Output B
	_ud(0), // ud of master gpib interface
	_LogFile() // Logfile
{

}

//Destructor
SourceMeter::~SourceMeter(){}

bool SourceMeter::GetOutputStatus(int smuX){
	if (smuX == 1)
	{
		return _outputStatusA;
	}
	else if (smuX ==2)
	{
		return _outputStatusB;
	}

	else{
		cout << "SourceMeter::GetOutputStatus(int smuX) - incorrect SMU!\n" << endl;
		this->_LogFile.Write("SourceMeter::GetOutputStatus(int smuX) - incorrect SMU!");
    	exit (EXIT_FAILURE);
	}
}

double SourceMeter::GetActualVoltage(int smuX){
	if (smuX == 1)
	{
		return _actualVoltageA;
	}
	else if (smuX ==2)
	{
		return _actualVoltageB;
	}

	else{
		cout << "SourceMeter::GetActualVoltage(int smuX) - incorrect SMU!\n" << endl;
		this->_LogFile.Write("SourceMeter::GetActualVoltage(int smuX) - incorrect SMU!");
    	exit (EXIT_FAILURE);
	}
}


int SourceMeter::GetUD(){
	return _ud;
}

LogDev& SourceMeter::GetLogFile(){

	return this->_LogFile;
}

void SourceMeter::Initialize(int masterUD, int pad){

	stringstream ss;
	ss << "SourceMeter_" << pad;
	this->_LogFile.Initialize(ss.str().c_str());

	cout << "START SourceMeter::Initialize" << endl;
	cout << "-----------------------------" << endl;
	this->_LogFile.Write("START SourceMeter::Initialize");
	this->_LogFile.Write("-----------------------------");
	cout << "Try to open SourceMeter device with pad=" << pad << endl;

	// int ibdev(int board_index, int pad, int sad, int timeout, int send_eoi, int eos);
	//_________________________________________________________________________________//
	// ibdev() is used to obtain a device descriptor, which can then be used by other functions in the library.
	// The argument board_index specifies which GPIB interface board the device is connected to. 
	// The pad and sad arguments specify the GPIB address of the device to be opened (see ibpad() and ibsad()).
	// The timeout for io operations is specified by timeout (see ibtmo()). 
	// If send_eoi is nonzero, then the EOI line will be asserted with the last byte sent during writes (see ibeot()).
	// Finally, the eos argument specifies the end-of-string character and whether or not its reception should terminate reads (see ibeos()).
	int ud=ibdev(0,pad,0,20,1,/*1024*/1034);

	if (iberr==0)
	{
		cout << "Success: ud="<< ud << endl;
		this->_LogFile.Write("Success!");
	}
	else{
		cout << "Initialization FAILED!" << endl;
		this->_LogFile.Write("Initialization FAILED!");
		exit (EXIT_FAILURE);
	}

	this->_ud=ud;

	// int ibln(int ud, int pad, int sad, short *found_listener);
	//____________________________________________________________//
	// ibln() checks for the presence of a device, by attempting to address it as a listener.
	// ud specifies the GPIB interface board which should check for listeners. If ud is a device descriptor, then the device's access board is used.
	// The GPIB address to check is specified by the pad and sad arguments. pad specifies the primary address, 0 through 30 are valid values.
	// sad gives the secondary address, and may be a value from 0x60 through 0x7e (96 through 126), or one of the constants NO_SAD or ALL_SAD. 
	// NO_SAD indicates that no secondary addressing is to be used, and ALL_SAD indicates that all secondary addresses should be checked.
	// If the board finds a listener at the specified GPIB address(es), then the variable specified by the pointer found_listener is set to a nonzero value.
	// If no listener is found, the variable is set to zero.
	// The board must be controller-in-charge to perform this function. 
	// Also, it must have the capability to monitor the NDAC bus line (see iblines()).
	short *foundLstn = new short;

	ibln(masterUD, pad, 0,foundLstn);
	if((*foundLstn)==0)
	{
	  cout << "Device not found for ud=" << ud << " and pad=" << pad << endl;
	  cout << "Initialization FAILED!" << endl;
	  this->_LogFile.Write("Initialization FAILED!");
	  return;
	}
	else{
		cout << "Listener found." << endl;
		this->_LogFile.Write("Listener found.");
	}

	// clear device
	int returnval=ibclr(ud);
	cout << "Device clear sent " << returnval << endl;

	//reset device
	this->ResetDevice();

	//reset each channel
	this->ResetChannel(1);
	this->ResetChannel(2);

	// this->SetSourceVoltage(1, "0");
	// this->SetSourceVoltage(2, "0");

	// this->_actualVoltageA = this->GetSourceVoltage(1);
	// this->_actualVoltageB = this->GetSourceVoltage(2);

	cout << "SourceMeter Initialization finished." << endl;
	cout << "------------------------------------" << endl;
	this->_LogFile.Write("SourceMeter Initialization finished.");
	this->_LogFile.Write("-----------------------------");

}


void SourceMeter::ResetDevice(){

	int returnval = ibwrt_string(this->_ud, "reset()");
	cout << "Set default settings: " << returnval << endl;
	this->_LogFile.Write("Set default settings.");

}

// smuX =1 channel A, smuX=2 channel B
// resets channel
void SourceMeter::ResetChannel(int smuX){

	if (smuX == 1)
	{
		int returnval = ibwrt_string(this->_ud, "smua.reset()");
		cout << "Reset SMUA: " << returnval << endl;
		this->_LogFile.Write("Reset SMUA.");
	}

	else if (smuX == 2)
	{
		int returnval = ibwrt_string(this->_ud, "smub.reset()");
		cout << "Reset SMUB: " << returnval << endl;
		this->_LogFile.Write("Reset SMUB.");
	}

	else {
    cout << "SourceMeter::ResetChannel(int smuX) - incorrect SMU!\n" << endl;
    this->_LogFile.Write("SourceMeter::ResetChannel(int smuX) - incorrect SMU!");
    exit (EXIT_FAILURE);
	}
	

}

// smuX =1 channel A, smuX=2 channel B
void SourceMeter::SelectVoltageFunction(int smuX){

	if (smuX == 1)
	{
		int returnval = ibwrt_string(this->_ud, "smua.source.func = smua.OUTPUT_DCVOLTS");
		cout << "Select voltage source function SMUA. " << returnval << endl;
		this->_LogFile.Write("Select voltage source function SMUA.");
	}

	else if (smuX == 2)
	{
		int returnval = ibwrt_string(this->_ud, "smub.source.func = smub.OUTPUT_DCVOLTS");
		cout << "Select voltage source function SMUB. " << returnval << endl;
		this->_LogFile.Write("Select voltage source function SMUB.");
	}

	else {
    cout << "SourceMeter::SelectVoltageFunction(int smuX) - incorrect SMU!\n" << endl;
    this->_LogFile.Write("SourceMeter::SelectVoltageFunction(int smuX) - incorrect SMU!");
    exit (EXIT_FAILURE);
	}
	

}

// smuX =1 channel A, smuX=2 channel B
void SourceMeter::SelectCurrentFunction(int smuX){

	if (smuX == 1)
	{
		int returnval = ibwrt_string(this->_ud, "smua.source.func = smua.OUTPUT_DCAMPS");
		cout << "Select current source function SMUA. " << returnval << endl;
		this->_LogFile.Write("Select current source function SMUA.");
	}

	else if (smuX == 2)
	{
		int returnval = ibwrt_string(this->_ud, "smub.source.func = smub.OUTPUT_DCAMPS");
		cout << "Select current source function SMUB. " << returnval << endl;
		this->_LogFile.Write("Select current source function SMUB.");
	}

	else {
    cout << "SourceMeter::SelectCurrentFunction(int smuX) - incorrect SMU!\n" << endl;
    this->_LogFile.Write("SourceMeter::SelectCurrentFunction(int smuX) - incorrect SMU!");
    exit (EXIT_FAILURE);
	}
	

}



// smuX =1 channel A, smuX=2 channel B
// turns Output on or off
void SourceMeter::SetOutputOnOff(int smuX, bool On){

	if (smuX == 1 && On == true)
	{
		int returnval = ibwrt_string(this->_ud, "smua.source.output = smua.OUTPUT_ON");
		_outputStatusA = 1;
		cout << "Set output on SMUA. " << returnval << endl;
		this->_LogFile.Write("Set output on SMUA.");
	}

	else if (smuX == 2 && On == true)
	{
		int returnval = ibwrt_string(this->_ud, "smub.source.output = smub.OUTPUT_ON");
		_outputStatusB = 1;
		cout << "Set output on SMUB. " << returnval << endl;
		this->_LogFile.Write("Set output on SMUB.");

	}

	else if (smuX == 1 && On == false)
	{
		int returnval = ibwrt_string(this->_ud, "smua.source.output = smua.OUTPUT_OFF");
		_outputStatusA = 0;
		cout << "Set output off SMUA. " << returnval << endl;
		this->_LogFile.Write("Set output off SMUA.");

	}

	else if (smuX == 2 && On == false)
	{
		int returnval = ibwrt_string(this->_ud, "smub.source.output = smub.OUTPUT_OFF");
		_outputStatusB = 0;
		cout << "Set output off SMUB. " << returnval << endl;
		this->_LogFile.Write("Set output off SMUB.");

	}

	else {
    cout << "SourceMeter::SetOutputOnOff(int smuX) - incorrect SMU!\n" << endl;
    this->_LogFile.Write("SourceMeter::SetOutputOnOff(int smuX) - incorrect SMU!");
    exit (EXIT_FAILURE);
	}
	

}

// smuX =1 channel A, smuX=2 channel B, in volt
// selects source range of voltage
void SourceMeter::SelectVoltageRange(int smuX, const string range){

	string command;
	stringstream out;
	if (smuX == 1)
	{
		command = "smua.source.rangev = " + range;
		int returnval = ibwrt_string(this->_ud, command);
		out << "Select voltage range SMUA to " << range << "V. " << returnval;
		cout << out.str() << endl;
		this->_LogFile.Write(out.str());

	}

	else if (smuX == 2)
	{
		command = "smub.source.rangev = " + range;
		int returnval = ibwrt_string(this->_ud, command);
		out << "Select voltage range SMUB to " << range << "V. " << returnval;
		cout << out.str() << endl;
		this->_LogFile.Write(out.str());
	}

	else {
    cout << "SourceMeter::SelectRange(int smuX) - incorrect SMU!\n" << endl;
    this->_LogFile.Write("SourceMeter::SelectRange(int smuX) - incorrect SMU!");
    exit (EXIT_FAILURE);
	}
	
}

// smuX =1 channel A, smuX=2 channel B, in volt
// selects source range of current
void SourceMeter::SelectCurrentRange(int smuX, const string range){

	string command;
	stringstream out;
	if (smuX == 1)
	{
		command = "smua.source.rangei = " + range;
		int returnval = ibwrt_string(this->_ud, command);
		out << "Select current range SMUA to " << range << "A. " << returnval;
		cout << out.str() << endl;
		this->_LogFile.Write(out.str());
	}

	else if (smuX == 2)
	{
		command = "smub.source.rangei = " + range;
		int returnval = ibwrt_string(this->_ud, command);
		out << "Select current range SMUB to " << range << "A. " << returnval;
		cout << out.str() << endl;
		this->_LogFile.Write(out.str());
	}

	else {
    cout << "SourceMeter::SelectRange(int smuX) - incorrect SMU!\n" << endl;
    this->_LogFile.Write("SourceMeter::SelectCurrentRange(int smuX) - incorrect SMU!");
    exit (EXIT_FAILURE);
	}
	
}



// smuX =1 channel A, smuX=2 channel B
void SourceMeter::SetCurrentLimit(int smuX, const string limit){

	string command;
	stringstream out;
	if (smuX == 1)
	{
		command = "smua.source.limiti = " + limit;
		int returnval = ibwrt_string(this->_ud, command);
		out << "Select current limit SMUA to " << limit << "A. " << returnval;
		cout << out.str() << endl;
		this->_LogFile.Write(out.str());
	}

	else if (smuX == 2)
	{
		command = "smub.source.limiti = " + limit;
		int returnval = ibwrt_string(this->_ud, command);
		out << "Select current limit SMUB to " << limit << "A. " << returnval;
		cout << out.str() << endl;
		this->_LogFile.Write(out.str());
	}

	else {
    cout << "SourceMeter::SetCurrentLimit(int smuX) - incorrect SMU!\n" << endl;
    this->_LogFile.Write("SourceMeter::SetCurrentLimit(int smuX) - incorrect SMU!");
    exit (EXIT_FAILURE);
	}
	
}

// smuX =1 channel A, smuX=2 channel B
void SourceMeter::SetVoltageLimit(int smuX, const string limit){

	string command;
	stringstream out;
	if (smuX == 1)
	{
		command = "smua.source.limitv = " + limit;
		int returnval = ibwrt_string(this->_ud, command);
		out << "Select voltage limit SMUA to " << limit << "V. " << returnval;
		cout << out.str() << endl;
		this->_LogFile.Write(out.str());
	}

	else if (smuX == 2)
	{
		command = "smub.source.limitv = " + limit;
		int returnval = ibwrt_string(this->_ud, command);
		out << "Select voltage limit SMUB to " << limit << "V. " << returnval;
		cout << out.str() << endl;
		this->_LogFile.Write(out.str());
	}

	else {
    cout << "SourceMeter::SetVoltageLimit(int smuX) - incorrect SMU!\n" << endl;
    this->_LogFile.Write("SourceMeter::SetVoltageLimit(int smuX) - incorrect SMU!");
    exit (EXIT_FAILURE);
	}
	
}

double SourceMeter::GetSourceVoltage(int smuX){

	stringstream out;
	if (smuX ==1)
	{
		int returnval = ibwrt_string(this->_ud, "print(smua.source.levelv)");
		this->_actualVoltageA = ibrd_double(this->_ud);
		out << "Read voltage level SMUA: " << this->_actualVoltageA << "V: " << returnval;
		cout << out.str() << endl;
		this->_LogFile.Write(out.str());
		return this->_actualVoltageA;
	}
	else if (smuX ==2)
	{
		int returnval = ibwrt_string(this->_ud, "print(smub.source.levelv)");
		this->_actualVoltageB = ibrd_double(this->_ud);
		out << "Read voltage level SMUB: " << this->_actualVoltageB << "V: " << returnval;
		cout << out.str() << endl;
		this->_LogFile.Write(out.str());
		return this->_actualVoltageB;
	}
	else {
    cout << "SourceMeter::GetSourceVoltage(int smuX) - incorrect SMU!\n" << endl;
    this->_LogFile.Write("SourceMeter::GetSourceVoltage(int smuX) - incorrect SMU!");
    exit (EXIT_FAILURE);
	}

}

// smuX =1 channel A, smuX=2 channel B
void SourceMeter::SetSourceVoltage(int smuX, const string level){

	string command;
	stringstream out;
	if (smuX == 1)
	{
		command = "smua.source.levelv = " + level;
		int returnval = ibwrt_string(this->_ud, command);
		out << "Select voltage level SMUA to " << level << "V. " << returnval;
		cout << out.str() << endl;
		this->_LogFile.Write(out.str());
		this->_actualVoltageA = atof(level.c_str());

	}

	else if (smuX == 2)
	{
		command = "smub.source.levelv = " + level;
		int returnval = ibwrt_string(this->_ud, command);
		out << "Select voltage level SMUB to " << level << "V. " << returnval;
		cout << out.str() << endl;
		this->_LogFile.Write(out.str());
		this->_actualVoltageB = atof(level.c_str());

	}

	else {
    cout << "SourceMeter::SetSourceVoltage(int smuX) - incorrect SMU!\n" << endl;
    this->_LogFile.Write("SourceMeter::SetSourceVoltage(int smuX) - incorrect SMU!");
    exit (EXIT_FAILURE);
	}
	
}

double SourceMeter::GetSourceCurrent(int smuX){

	stringstream out;
	if (smuX == 1)
	{
		int returnval = ibwrt_string(this->_ud, "print(smua.source.leveli)");
		this->_actualCurrentA = ibrd_double(this->_ud);
		out << "Read current level SMUA: " << this->_actualCurrentA << "A: " << returnval;
		cout << out.str() << endl;
		this->_LogFile.Write(out.str());
		return this->_actualCurrentA;
	}
	else if (smuX == 2)
	{
		int returnval = ibwrt_string(this->_ud, "print(smub.source.leveli)");
		this->_actualCurrentB = ibrd_double(this->_ud);
		out << "Read current level SMUB: " << this->_actualCurrentB << "A: " << returnval;
		cout << out.str() << endl;
		this->_LogFile.Write(out.str());
		return this->_actualCurrentB;
	}
	else {
    cout << "SourceMeter::GetSourceCurrent(int smuX) - incorrect SMU!\n" << endl;
    this->_LogFile.Write("SourceMeter::GetSourceCurrent(int smuX) - incorrect SMU!");
    exit (EXIT_FAILURE);
	}

}

// smuX =1 channel A, smuX=2 channel B
void SourceMeter::SetSourceCurrent(int smuX, const string level){

	string command;
	stringstream out;
	if (smuX == 1)
	{
		command = "smua.source.leveli = " + level;
		int returnval = ibwrt_string(this->_ud, command);
		out << "Select current level SMUA to " << level << "A. " << returnval;
		cout << out.str() << endl;
		this->_LogFile.Write(out.str());
		this->_actualCurrentA = atof(level.c_str());

	}

	else if (smuX == 2)
	{
		command = "smub.source.leveli = " + level;
		int returnval = ibwrt_string(this->_ud, command);
		out << "Select current level SMUB to " << level << "A. " << returnval;
		cout << out.str() << endl;
		this->_LogFile.Write(out.str());
		this->_actualCurrentB = atof(level.c_str());

	}

	else {
    cout << "SourceMeter::SetSourceCurrent(int smuX) - incorrect SMU!\n" << endl;
    this->_LogFile.Write("SourceMeter::SetSourceCurrent(int smuX) - incorrect SMU!");
    exit (EXIT_FAILURE);
	}
	
}

vector<double> SourceMeter::MeasureIV(int smuX){

	vector<double> iv(2,0);

	if (smuX ==1)
	{
		int returnval = ibwrt_string(this->_ud, "print(smua.measure.iv())");
	
		ibrd(this->_ud, this->_readA, 70);

		stringstream ss;
		ss << this->_readA;
		ss >> iv[0] >> iv[1];

		cout << "Measured I: " << iv[0] << "A and V: " << iv[1] << "V. " << returnval << endl;

		return iv;

	}
	else if (smuX ==2)
	{
		int returnval = ibwrt_string(this->_ud, "print(smub.measure.iv())");
		
		ibrd(this->_ud, this->_readB, 70);

		stringstream ss;
		ss << this->_readB;
		ss >> iv[0] >> iv[1];

		cout << "Measured I: " << iv[0] << "A and V: " << iv[1] << "V. " << returnval << endl;

		return iv;
	}

	else {
    cout << "SourceMeter::MeasureIV(int smuX) - incorrect SMU!\n" << endl;
    this->_LogFile.Write("SourceMeter::MeasureIV(int smuX) - incorrect SMU!");
    exit (EXIT_FAILURE);
	}

}

double SourceMeter::MeasureI(int smuX){

	double curr;

	if (smuX ==1)
	{
		int returnval = ibwrt_string(this->_ud, "print(smua.measure.i())");
	
		ibrd(this->_ud, this->_readA, 70);

		stringstream ss;
		ss << this->_readA;
		ss >> curr;

		cout << "Measured I: " << curr << "A " << returnval << endl;

		return curr;

	}
	else if (smuX ==2)
	{
		int returnval = ibwrt_string(this->_ud, "print(smub.measure.i())");
		
		ibrd(this->_ud, this->_readB, 70);

		stringstream ss;
		ss << this->_readB;
		ss >> curr;

		cout << "Measured I: " << curr << "A " << returnval << endl;

		return curr;
	}

	else {
    cout << "SourceMeter::MeasureI(int smuX) - incorrect SMU!\n" << endl;
    this->_LogFile.Write("SourceMeter::MeasureI(int smuX) - incorrect SMU!");
    exit (EXIT_FAILURE);
	}

}

double SourceMeter::MeasureV(int smuX){

	double volt;

	if (smuX ==1)
	{
		int returnval = ibwrt_string(this->_ud, "print(smua.measure.v())");
	
		ibrd(this->_ud, this->_readA, 70);

		stringstream ss;
		ss << this->_readA;
		ss >> volt;

		cout << "Measured V: " << volt << "V " << returnval << endl;

		return volt;

	}
	else if (smuX ==2)
	{
		int returnval = ibwrt_string(this->_ud, "print(smub.measure.v())");
		
		ibrd(this->_ud, this->_readB, 70);

		stringstream ss;
		ss << this->_readB;
		ss >> volt;

		cout << "Measured V: " << volt << "V " << returnval << endl;

		return volt;
	}

	else {
    cout << "SourceMeter::MeasureV(int smuX) - incorrect SMU!\n" << endl;
    this->_LogFile.Write("SourceMeter::MeasureV(int smuX) - incorrect SMU!");
    exit (EXIT_FAILURE);
	}

}










