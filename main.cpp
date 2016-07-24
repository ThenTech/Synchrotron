#include <iostream>
#include <stdio.h>
#include <vector>

#define TEST_PERFORMANCE
#define TEST_SET
#define ELEMENTS	100000
#define TIMES		10
/*
 *	Note: signal.emit() with ELEMENTS outputs ~= signal.emit() to slot with ELEMENTS inputs
 */
/*	ELEMENTS =   100 000	TIMES =    10
	std::set<> results: ~more memory usage
		Test addOutputs    :: Average time:    91 milliseconds :: (min=    78, max=    93)
		Test emit          :: Average time:     3 milliseconds :: (min=     0, max=    15)
		Test removeOutputs :: Average time:    84 milliseconds :: (min=    78, max=    94)

	std::forward_list<> results: >>>>time on remove usage
		Test addOutputs    :: Average time:    78 milliseconds :: (min=    78, max=    78)
		Test emit          :: Average time:     0 milliseconds :: (min=     0, max=     0)
		Test removeOutputs :: Average time: 11187 milliseconds :: (min= 11071, max= 11316)
*/
/*	ELEMENTS = 1 000 000	TIMES =    10
	std::set<> results: ~more memory usage
		Test addOutputs    :: Average time:   938 milliseconds :: (min=   922, max=   969)
		Test emit 		   :: Average time:    38 milliseconds :: (min=    31, max=    46)
		Test removeOutputs :: Average time:   848 milliseconds :: (min=   842, max=   860)

	std::forward_list<> results:
		5+ hours?!
		==>	std::forward_list<>::remove() is of complexity O(n)
			whereas std::set<>::erase() uses a LUT with keys generated on insert() and is therefore O(1).
*/
/*
	ELEMENTS = 1 000		TIMES = 1 000
		std::set<> results:				Total time: 1638 ms	(others 0 ms)
		std::forward_list<> results:	Total time: 2449 ms (others 0 ms, except remove: 1 ms)
*/
#ifdef TEST_SET
	#include "SynchrotronComponent.hpp"
#else
	#include "SynchrotronComponentList.hpp"
#endif // TEST_SET

using namespace Synchrotron;

void printResults(std::vector<size_t>& v) {
	size_t min = 1e6, max = 0, sum = 0, size = v.size();
	for(size_t i = 0; i < size; i++) {
		if (v.at(i) < min) min = v.at(i);
		if (v.at(i) > max) max = v.at(i);
		sum += v.at(i);
	}

	printf("Average time: %4d milliseconds :: (min= %4d, max= %4d)\n", (sum / size), min, max);
}

int main() {
#ifndef TEST_PERFORMANCE
	SynchrotronComponent<4> slot(1);
	SynchrotronComponent<4> signal(2);

	slot.addInput(signal);
	//signal.addOutput(slot);

	//slot.tick();
    //slot.emit();

	//signal.tick();
	signal.emit();
	std::cout << slot.getState() << std::endl;

	SynchrotronComponent<4> dupslot(slot);
	std::cout << dupslot.getState() << std::endl;
	signal.emit();
	std::cout << dupslot.getState() << std::endl;
#else
	std::cout << "Starting tests...\n";
	std::vector<size_t> runtimes_addOutputs, runtimes_Emit, runtimes_removeOutputs;

	SynchrotronComponent<16> 				signalprovider(0x8000);
	std::vector<SynchrotronComponent<16>*>	slots;

	for (int i = ELEMENTS; i--;) {
		slots.push_back(new SynchrotronComponent<16>(i % 0xFFFF));
	}

	auto start = std::chrono::high_resolution_clock::now();

	for (int i = TIMES; i--;) {
	////////////////////////////////////////////////////////////////////////////////
		auto t1 = std::chrono::high_resolution_clock::now();

		for(auto& s : slots)
			signalprovider.addOutput(*s);

		auto t2 = std::chrono::high_resolution_clock::now();
		runtimes_addOutputs.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count());
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
		t1 = std::chrono::high_resolution_clock::now();

		signalprovider.emit();

		t2 = std::chrono::high_resolution_clock::now();
		runtimes_Emit.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count());
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
		t1 = std::chrono::high_resolution_clock::now();

		for(auto& s : slots)
			signalprovider.removeOutput(*s);

		t2 = std::chrono::high_resolution_clock::now();
		runtimes_removeOutputs.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count());
	////////////////////////////////////////////////////////////////////////////////
	}

	auto end = std::chrono::high_resolution_clock::now();


	std::cout << "Test addOutputs    :: ";
	printResults(runtimes_addOutputs);
	std::cout << "Test emit          :: ";
	printResults(runtimes_Emit);
	std::cout << "Test removeOutputs :: ";
	printResults(runtimes_removeOutputs);
	std::cout << "Total time taken  :: " << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() << " milliseconds" << std::endl;

	for(auto& s : slots) delete s;
	slots.clear();
#endif // TEST_PERFORMANCE

	return 0;
}
