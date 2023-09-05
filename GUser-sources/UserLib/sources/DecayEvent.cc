#include "DecayEvent.h"

DecayEvent::DecayEvent(): Event() {}

DecayEvent::~DecayEvent(){}

void DecayEvent::Clear(){
	fDssdEvent.Clear();
}
