/*

SourecMeter.h


Created by Janine Müller on 05.10.2016


*/

#ifndef ____SourceMeter__
#define ____SourceMeter__

#include <string>
#include <vector>
#include "../LogFileDev/LogDev.h"

class SourceMeter
{
public:
	SourceMeter();
	~SourceMeter();

	bool GetOutputStatus(int smuX);
	double GetActualVoltage(int smuX);
	int GetUD();
	LogDev& GetLogFile();

	void Initialize(int masterUD, int pad);

	void ResetDevice();
	void ResetChannel(int smuX);
	void SelectVoltageFunction(int smuX);
	void SelectCurrentFunction(int smuX);
	void SetOutputOnOff(int smuX, bool On = true);
	void SelectCurrentRange(int smuX, const std::string range);
	void SelectVoltageRange(int smuX, const std::string range);
	void SetCurrentLimit(int smuX, const std::string limit);
	void SetVoltageLimit(int smuX, const std::string limit);

	double GetSourceVoltage(int smuX);
	void SetSourceVoltage(int smuX, const std::string level);
	double GetSourceCurrent(int smuX);
	void SetSourceCurrent(int smuX, const std::string level);

	double MeasureI(int smuX);
	double MeasureV(int smuX);
	std::vector<double> MeasureIV(int smuX);

private:
	bool _outputStatusA; 
	bool _outputStatusB;

	double _actualVoltageA;
	double _actualVoltageB;

	double _actualCurrentA;
	double _actualCurrentB;

	int _ud;

	char _readA[70];
	char _readB[70];

	LogDev _LogFile;


};


#endif /* defined(____SourceMeter__) */