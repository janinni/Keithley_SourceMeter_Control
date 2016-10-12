CXX		= g++
CXXFLAGS	= -O2 -Wall 
LDFLAGS		= -lgpib
SRCFILES	= SourceMeter.cpp SourceMeterMain.cpp ../gpib/gpib.cpp
OBJFILES    = SourceMeter.o SourceMeterMain.o ../gpib/gpib.o

SourceMeter:		$(OBJFILES)
		$(CXX) $(OBJFILES) $(LDFLAGS) -o $@

clean:
		rm -f $(OBJFILES) SourceMeter 

SourceMeter.o:	SourceMeter.cpp SourceMeter.h ../gpib/gpib.h

gpib.o:	../gpib/gpib.cpp ../gpib/gpib.h

SourceMeterMain.o: SourceMeterMain.cpp SourceMeter.h ../gpib/gpib.h
