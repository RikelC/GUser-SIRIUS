
# Extensions des fichiers
ObjSuf        = o
SrcSuf        = cxx
ExeSuf        =
DllSuf        = so
OutPutOpt     = -o 

# Compilateur et options
CXX           = g++
CXXFLAGS      = -O -Wall -fPIC -g
LD            = g++
SOFLAGS       = -shared 
LDFLAGS       = -O 


# Options de ROOT
ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --libs)
ROOTGLIBS    := $(shell root-config --glibs)

#CXXFLAGS     += $(ROOTCFLAGS) $(OPTFLAGS)
CXXFLAGS     += $(ROOTCFLAGS)
LIBS          = $(ROOTLIBS) $(SYSLIBS) 
GLIBS         = $(ROOTGLIBS) $(SYSLIBS) 


#------------------------------------------------------------------------------
SHARELIB      = libSiriusData.so 
all:            $(SHARELIB)
#------------------------------------------------------------------------------



libSiriusData.so:	siriusData.o   siriusDataDict.o
		$(LD) $(SOFLAGS) $^ $(OutPutOpt) $@

siriusDataDict.cxx:	siriusData.h
		rootcint -f $@ -c $^
#-----------


clean:
	@echo "Clean :  rm -f core  *.o *Dict* *.so"
	@rm -f core  *.o *Dict*.* *.so

distclean:
	  make clean; rm -f *.so

.SUFFIXES: .$(SrcSuf)

###

.$(SrcSuf).$(ObjSuf):
	$(CXX) $(CXXFLAGS) -c $<

# dependances
sirusData.o:	siriusData.cxx	siriusData.h

