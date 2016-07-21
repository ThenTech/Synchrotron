#include <iostream>

#include "SynchrotronComponent.hpp"

using namespace Synchrotron;

int main() {

	SynchrotronComponent<2> slot(1);
	SynchrotronComponent<2> signal(2);

	//std::cout << "add success: " << slot.addInput(&signal) << std::endl;
	std::cout << "add success: " << signal.addOutput(&slot) << std::endl;


	//slot.tick();
    //slot.emit();

	//signal.tick();
	signal.emit();

	return 0;
}
