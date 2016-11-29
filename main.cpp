#include <iostream>
#include <stdio.h>
#include <vector>

#define BSTR(STRB)	( (STRB) ? "true" : "false" )

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
/*	ELEMENTS = 1 000		TIMES = 1 000
		std::set<> results:				Total time: 1638 ms	(others 0 ms)
		std::forward_list<> results:	Total time: 2449 ms (others 0 ms, except remove: 1 ms)
*/

//#define TEST_PERFORMANCE
#define ELEMENTS	10000
#define TIMES		10
#define USE_SYNC	6

#include "SynchrotronComponent.hpp"				// 1
#include "SynchrotronComponentList.hpp"			// 2
#include "SynchrotronComponentFList.hpp"		// 3
#include "SynchrotronComponentVector.hpp"		// 4
#include "SynchrotronComponentSetInsertEnd.hpp"	// 5
#include "SynchrotronComponentSetSort.hpp"		// 6

using namespace Synchrotron;

#if USE_SYNC == 1
	typedef SynchrotronComponent<16> SYNCHROTRON;
#elif USE_SYNC == 2
	typedef SynchrotronComponentList<16> SYNCHROTRON;
#elif USE_SYNC == 3
	typedef SynchrotronComponentFList<16> SYNCHROTRON;
#elif USE_SYNC == 4
	typedef SynchrotronComponentVector<16> SYNCHROTRON;
#elif USE_SYNC == 5
	typedef SynchrotronComponentSetInsertEnd<16> SYNCHROTRON;
#elif USE_SYNC == 6
	typedef SynchrotronComponentSetSort<16> SYNCHROTRON;
#endif

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
	SYNCHROTRON slot(1);
	SYNCHROTRON signal(2);

	slot.addInput(signal);
	//signal.addOutput(slot);

	//slot.tick();
    //slot.emit();

	//signal.tick();
	signal.emit();
	std::cout << "Slot : " << slot.getState() << " Expected: " << "0011" << std::endl;

	SYNCHROTRON dupslot(slot);
	std::cout << "Dupl : " << dupslot.getState() << " Expected: " << "0000" << std::endl;
	signal.emit();
	std::cout << "Dupl : " << dupslot.getState() << " Expected: " << "0010" << std::endl;

	SYNCHROTRON s1(8), s2(8), s3(8), s4(8), s5(8);

#if USE_SYNC != 3
	std::cout << "Size : " << signal.getOutputs().size() << " Expected: " << 2 << std::endl;
#endif

	signal.addOutput( {&s1, &s2, &s3, &s4} );

#if USE_SYNC != 3
	std::cout << "Size : " << signal.getOutputs().size() << " Expected: " << 6 << std::endl;
#endif

	std::cout << "Raw pointers in sequence of added:" << std::endl;
	std::cout << "p = " << &slot << std::endl;
	std::cout << "p = " << &dupslot << std::endl;
	std::cout << "p = " << &s1 << std::endl;
	std::cout << "p = " << &s2 << std::endl;
	std::cout << "p = " << &s3 << std::endl;
	std::cout << "p = " << &s4 << std::endl;

	std::cout << "Raw pointers in signal.outputs: (should be same order as above)" << std::endl;
	SYNCHROTRON *end;
	for (auto con : signal.getOutputs()) {
		std::cout << "q = " << con << std::endl;
		end = con;
	}

	#if USE_SYNC == 3
		std::cout << "Kept order on init_list: begin = " << BSTR(*signal.getOutputs().begin() == &slot) << std::endl
				  << "                         end   = " << BSTR(end == &s4)
				  << std::endl;
	#else
		std::cout << "Kept order on init_list: begin = " << BSTR(*signal.getOutputs().begin() == &slot) << std::endl
				  << "                         end   = " << BSTR(*signal.getOutputs().crbegin() == &s4)
				  << std::endl;
	#endif

	end = nullptr;

	signal.addOutput(s5);

	#if USE_SYNC == 3
		std::cout << "Kept order on add      : begin = " << BSTR(*signal.getOutputs().begin() == &slot) << std::endl
				  << "                         end   = " << BSTR(end == &s5)
				  << std::endl;
	#else
		std::cout << "Kept order on add      : begin = " << BSTR(*signal.getOutputs().begin() == &slot) << std::endl
				  << "                         end   = " << BSTR(*signal.getOutputs().crbegin() == &s5)
				  << std::endl;
	#endif

#if USE_SYNC != 3
	size_t nr = signal.getOutputs().size();
	signal.addOutput(s5);
	std::cout << "Won't allow duplicates: " << BSTR(nr == signal.getOutputs().size()) << std::endl;
#else
	std::cout << "Won't allow duplicates: " << BSTR(0) << std::endl;
#endif

    signal.emit();
	std::cout << "state: " << s1.getState() << " Expected: " << "1010" << std::endl;

#else
	std::cout << "Starting tests...\n";
	std::vector<size_t> runtimes_addOutputs, runtimes_Emit, runtimes_removeOutputs;

	SYNCHROTRON				 	signalprovider(0x8000);
	std::vector<SYNCHROTRON*>	slots;

	for (int i = ELEMENTS; i--;) {
		slots.push_back(new SYNCHROTRON(i % 0xFFFF));
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
	std::cout << "Total Size    :: " << (sizeof(signalprovider) + 10000*sizeof(SYNCHROTRON)) << std::endl;
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
