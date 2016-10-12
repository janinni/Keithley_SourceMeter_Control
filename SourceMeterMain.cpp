/*

SourecMeterMain.cpp

Created by Janine Müller on 05.10.2016

*/

#include <iostream>

#include "SourceMeter.h"
#include "../gpib/gpib.h"



int main(int argc, char const *argv[])
{

	int masterUD = InitializeMaster();

	SourceMeter* Keithley = new SourceMeter();

	Keithley->Initialize(masterUD, 26, 10, 10);

	Keithley->SelectSourceFunction(1, false);
	Keithley->SetLimit(1,"2", true);	

	Keithley->SetOutputOnOff(1,true);
	Keithley->SetSourceCurrent(1, "0.2");
	
	double Current = Keithley->GetSourceCurrent(1);

	cout << Current << endl;
	
	vector<double> measure (2,0);
	
	measure = Keithley->MeasureIV(1);

	cout << measure[0] << "\t" << measure[1] << endl;
	
	sleep(10);

	Keithley->SetSourceCurrent(1,"0");
	Keithley->SetOutputOnOff(1,false);
	return 0;
}
