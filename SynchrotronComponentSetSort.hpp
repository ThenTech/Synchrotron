#ifndef SYNCHROTRONCOMPONENTSETSORT_HPP
#define SYNCHROTRONCOMPONENTSETSORT_HPP

#include <iostream> // For testing for now

#include "SynchrotronComponent.hpp"
#include <bitset>
#include <set>

namespace Synchrotron {

	template <size_t bit_width>
	class SynchrotronComponentSetSort;

	struct sync_cmp {
		template <size_t bit_width>
		bool operator() (const SynchrotronComponentSetSort<bit_width>* lhs, const SynchrotronComponentSetSort<bit_width>* rhs) const{
			// static id in every SynchrotronComponentSetSort ? and sort on this id?
			(void) lhs;
			(void) rhs;

			//std::cout << "lhs: " << lhs << ", rhs: " << rhs << std::endl;

			return lhs == rhs;
		}
	};

	/** \brief
	 *	SynchrotronComponent is the base for all components,
	 *	offering in and output connections to other SynchrotronComponent.
	 *
	 *	\param	bit_width
	 *		This template argument specifies the width of the internal bitset state.
	 */
	template <size_t bit_width>
	class SynchrotronComponentSetSort : public Mutex {
		private:
			/**	\brief
			 *	The current internal state of bits in this component (default output).
			 */
			std::bitset<bit_width> state;

			/**	\brief
			 *	**Slots == outputs**
			 *
			 *		Emit this.signal to subscribers in slotOutput.
			 */
			std::set<SynchrotronComponentSetSort*, sync_cmp> slotOutput;

			/**	\brief
			 *	**Signals == inputs**
			 *
			 *		Receive tick()s from these subscriptions in signalInput.
			 */
			std::set<SynchrotronComponentSetSort*, sync_cmp> signalInput;

			/**	\brief	Connect a new slot s:
			 *		* Add s to this SynchrotronComponentSetSort's outputs.
			 *		* Add this to s's inputs.
			 *
			 *	\param	s
			 *		The SynchrotronComponentSetSort to connect.
			 */
			inline void connectSlot(SynchrotronComponentSetSort* s) {
				//LockBlock lock(this);

				this->slotOutput.insert(this->slotOutput.end(), s);
				s->signalInput.insert(s->signalInput.end(),this);
			}

			/**	\brief	Disconnect a slot s:
			 *		* Remove s from this SynchrotronComponentSetSort's outputs.
			 *		* Remove this from s's inputs.
			 *
			 *	\param	s
			 *		The SynchrotronComponentSetSort to disconnect.
			 */
			inline void disconnectSlot(SynchrotronComponentSetSort* s) {
				//LockBlock lock(this);

				this->slotOutput.erase(s);
				s->signalInput.erase(this);
			}

		public:
			/** \brief	Default constructor
			 *
			 *	\param	initial_value
			 *		The initial state of the internal bitset.
			 *	\param	bit_width
			 *		The size of the internal width of the bitset.
			 */
			SynchrotronComponentSetSort(size_t initial_value = 0) : state(initial_value) {}

			/**	\brief
			 *	Copy constructor
			 *	*	Duplicates signal subscriptions (inputs)
			 *	*	Optionally also duplicates slot connections (outputs)
			 *
			 *	\param	sc const
			 *		The other SynchrotronComponentSetSort to duplicate the connections from.
			 *	\param	duplicateAll_IO
			 *		Specifies whether to only copy inputs (false) or outputs as well (true).
			 */
			SynchrotronComponentSetSort(const SynchrotronComponentSetSort& sc, bool duplicateAll_IO = false) : SynchrotronComponentSetSort() {
				//LockBlock lock(this);

				// Copy subscriptions
				for(auto& sender : sc.signalInput) {
					this->addInput(*sender);
				}

				if (duplicateAll_IO) {
					// Copy subscribers
					for(auto& connection : sc.slotOutput) {
						this->addOutput(*connection);
					}
				}
			}

			/**	\brief
			 *	Connection constructor
			 *	*	Adds signal subscriptions from inputList
			 *	*	Optionally adds slot subscribers from outputList
			 *
			 *	\param	inputList
			 *		The list of SynchrotronComponents to connect as input.
			 *	\param	outputList
			 *		The list of SynchrotronComponents to connect as output..
			 */
			SynchrotronComponentSetSort(std::initializer_list<SynchrotronComponentSetSort*> inputList,
								 std::initializer_list<SynchrotronComponentSetSort*> outputList = {})
									: SynchrotronComponentSetSort() {
				this->addInput(inputList);
				this->addOutput(outputList);
			}

			/** \brief	Default destructor
			 *
			 *		When called, will disconnect all in and output connections to this SynchrotronComponentSetSort.
			 */
			~SynchrotronComponentSetSort() {
				LockBlock lock(this);

				// Disconnect all Slots
				for(auto& connection : this->slotOutput) {
					connection->signalInput.erase(this);
					//delete connection; //?
				}

				// Disconnect all Signals
				for(auto &sender: this->signalInput) {
					sender->slotOutput.erase(this);
				}

				this->slotOutput.clear();
				this->signalInput.clear();
			}

			/**	\brief	Gets this SynchrotronComponentSetSort's bit width.
			 *
			 *	\return	size_t
			 *      Returns the bit width of the internal bitset.
			 */
			size_t getBitWidth() const {
				return bit_width;
			}

//			/* No real use since function cannot be called with different size SynchrotronComponentSetSorts */
//			/* Maybe viable when SynchrotronComponentSetSort has different in and output sizes */
//			/*	\brief	Compare this bit width to that of other.
//             *
//             *	\param	other
//			 *		The other SynchrotronComponentSetSort to check.
//			 *
//             *	\return	bool
//             *      Returns whether the widths match.
//             */
//			inline bool hasSameWidth(SynchrotronComponentSetSort& other) {
//				return this->getBitWidth() == other.getBitWidth();
//			}

			/**	\brief	Gets this SynchrotronComponentSetSort's state.
			 *
			 *	\return	std::bitset<bit_width>
			 *      Returns the internal bitset.
			 */
			inline std::bitset<bit_width> getState() const {
				return this->state;
			}

			/**	\brief	Gets the SynchrotronComponentSetSort's input connections.
			 *
			 *	\return	std::set<SynchrotronComponentSetSort*>&
			 *      Returns a reference set to this SynchrotronComponentSetSort's inputs.
			 */
			const std::set<SynchrotronComponentSetSort*, sync_cmp>& getIputs() const {
				return this->signalInput;
			}

			/**	\brief	Gets the SynchrotronComponentSetSort's output connections.
			 *
			 *	\return	std::set<SynchrotronComponentSetSort*>&
			 *      Returns a reference set to this SynchrotronComponentSetSort's outputs.
			 */
			const std::set<SynchrotronComponentSetSort*, sync_cmp>& getOutputs() const {
				return this->slotOutput;
			}

			/**	\brief	Adds/Connects a new input to this SynchrotronComponentSetSort.
			 *
			 *	**Ensures both way connection will be made:**
			 *	This will have input added to its inputs and input will have this added to its outputs.
			 *
			 *	\param	input
			 *		The SynchrotronComponentSetSort to connect as input.
			 */
			void addInput(SynchrotronComponentSetSort& input) {
				LockBlock lock(this);

				// deprecated? //if (!this->hasSameWidth(input)) return false;
				input.connectSlot(this);
			}

			/**	\brief	Adds/Connects a list of new inputs to this SynchrotronComponent.
			 *
			 *	Calls addInput() on each SynchrotronComponent* in inputList.
			 *
			 *	\param	inputList
			 *		The list of SynchrotronComponents to connect as input.
			 */
			void addInput(std::initializer_list<SynchrotronComponentSetSort*> inputList) {
				for(auto connection : inputList)
					this->addInput(*connection);
			}

			/**	\brief	Removes/Disconnects an input to this SynchrotronComponentSetSort.
			 *
			 *	**Ensures both way connection will be removed:**
			 *	This will have input removed from its inputs and input will have this removed from its outputs.
			 *
			 *	\param	input
			 *		The SynchrotronComponentSetSort to disconnect as input.
			 */
			void removeInput(SynchrotronComponentSetSort& input) {
				LockBlock lock(this);

				input.disconnectSlot(this);
			}

			/**	\brief	Adds/Connects a new output to this SynchrotronComponentSetSort.
			 *
			 *	**Ensures both way connection will be made:**
			 *	This will have output added to its outputs and output will have this added to its inputs.
			 *
			 *	\param	output
			 *		The SynchrotronComponentSetSort to connect as output.
			 */
			void addOutput(SynchrotronComponentSetSort& output) {
				LockBlock lock(this);

				// deprecated? //if (!this->hasSameWidth(*output)) return false;
				this->connectSlot(&output);
			}

			/**	\brief	Adds/Connects a list of new outputs to this SynchrotronComponent.
			 *
			 *	Calls addOutput() on each SynchrotronComponent* in outputList.
			 *
			 *	\param	outputList
			 *		The list of SynchrotronComponents to connect as output.
			 */
			void addOutput(std::initializer_list<SynchrotronComponentSetSort*> outputList) {
				for(auto connection : outputList)
					this->addOutput(*connection);
			}

			/**	\brief	Removes/Disconnects an output to this SynchrotronComponentSetSort.
			 *
			 *	**Ensures both way connection will be removed:**
			 *	This will have output removed from its output and output will have this removed from its inputs.
			 *
			 *	\param	output
			 *		The SynchrotronComponentSetSort to disconnect as output.
			 */
			void removeOutput(SynchrotronComponentSetSort& output) {
				LockBlock lock(this);

				this->disconnectSlot(&output);
			}

			/**	\brief	The tick() method will be called when one of this SynchrotronComponentSetSort's inputs issues an emit().
			 *
			 *	\return	virtual void
			 *		This method should be implemented by a derived class.
			 */
			virtual void tick() {
				//LockBlock lock(this);
				std::bitset<bit_width> prevState = this->state;

				//std::cout << "Ticked\n";
				for(auto& connection : this->signalInput) {
					// Change this line to change the logic applied on the states:
					this->state |= ((SynchrotronComponentSetSort*) connection)->getState();
				}

				// Directly emit changes to subscribers on change
				if (prevState != this->state)
					this->emit();
			}

			/**	\brief	The emit() method will be called after a tick() completes to ensure the flow of new data.
			 *
			 *	Loops over all outputs and calls tick().
			 *
			 *	\return	virtual void
			 *		This method can be re-implemented by a derived class.
			 */
			virtual inline void emit() {
				//LockBlock lock(this);

				for(auto& connection : this->slotOutput) {
					connection->tick();
				}
				//std::cout << "Emitted\n";
			}
	};

}


#endif // SYNCHROTRONCOMPONENTSETSORT_HPP
