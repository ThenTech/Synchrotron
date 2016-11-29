#ifndef SYNCHROTRONCOMPONENTSETINSERTEND_HPP
#define SYNCHROTRONCOMPONENTSETINSERTEND_HPP


#include <iostream> // For testing for now

#include "SynchrotronComponent.hpp"
#include <bitset>
#include <set>

namespace Synchrotron {

	/** \brief
	 *	SynchrotronComponent is the base for all components,
	 *	offering in and output connections to other SynchrotronComponent.
	 *
	 *	\param	bit_width
	 *		This template argument specifies the width of the internal bitset state.
	 */
	template <size_t bit_width>
	class SynchrotronComponentSetInsertEnd : public Mutex {
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
			std::set<SynchrotronComponentSetInsertEnd*> slotOutput;

			/**	\brief
			 *	**Signals == inputs**
			 *
			 *		Receive tick()s from these subscriptions in signalInput.
			 */
			std::set<SynchrotronComponentSetInsertEnd*> signalInput;

			/**	\brief	Connect a new slot s:
			 *		* Add s to this SynchrotronComponentSetInsertEnd's outputs.
			 *		* Add this to s's inputs.
			 *
			 *	\param	s
			 *		The SynchrotronComponentSetInsertEnd to connect.
			 */
			inline void connectSlot(SynchrotronComponentSetInsertEnd* s) {
				//LockBlock lock(this);

				this->slotOutput.insert(this->slotOutput.end(), s);
				s->signalInput.insert(s->signalInput.end(),this);
			}

			/**	\brief	Disconnect a slot s:
			 *		* Remove s from this SynchrotronComponentSetInsertEnd's outputs.
			 *		* Remove this from s's inputs.
			 *
			 *	\param	s
			 *		The SynchrotronComponentSetInsertEnd to disconnect.
			 */
			inline void disconnectSlot(SynchrotronComponentSetInsertEnd* s) {
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
			SynchrotronComponentSetInsertEnd(size_t initial_value = 0) : state(initial_value) {}

			/**	\brief
			 *	Copy constructor
			 *	*	Duplicates signal subscriptions (inputs)
			 *	*	Optionally also duplicates slot connections (outputs)
			 *
			 *	\param	sc const
			 *		The other SynchrotronComponentSetInsertEnd to duplicate the connections from.
			 *	\param	duplicateAll_IO
			 *		Specifies whether to only copy inputs (false) or outputs as well (true).
			 */
			SynchrotronComponentSetInsertEnd(const SynchrotronComponentSetInsertEnd& sc, bool duplicateAll_IO = false) : SynchrotronComponentSetInsertEnd() {
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
			SynchrotronComponentSetInsertEnd(std::initializer_list<SynchrotronComponentSetInsertEnd*> inputList,
								 std::initializer_list<SynchrotronComponentSetInsertEnd*> outputList = {})
									: SynchrotronComponentSetInsertEnd() {
				this->addInput(inputList);
				this->addOutput(outputList);
			}

			/** \brief	Default destructor
			 *
			 *		When called, will disconnect all in and output connections to this SynchrotronComponentSetInsertEnd.
			 */
			~SynchrotronComponentSetInsertEnd() {
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

			/**	\brief	Gets this SynchrotronComponentSetInsertEnd's bit width.
			 *
			 *	\return	size_t
			 *      Returns the bit width of the internal bitset.
			 */
			size_t getBitWidth() const {
				return bit_width;
			}

//			/* No real use since function cannot be called with different size SynchrotronComponentSetInsertEnds */
//			/* Maybe viable when SynchrotronComponentSetInsertEnd has different in and output sizes */
//			/*	\brief	Compare this bit width to that of other.
//             *
//             *	\param	other
//			 *		The other SynchrotronComponentSetInsertEnd to check.
//			 *
//             *	\return	bool
//             *      Returns whether the widths match.
//             */
//			inline bool hasSameWidth(SynchrotronComponentSetInsertEnd& other) {
//				return this->getBitWidth() == other.getBitWidth();
//			}

			/**	\brief	Gets this SynchrotronComponentSetInsertEnd's state.
			 *
			 *	\return	std::bitset<bit_width>
			 *      Returns the internal bitset.
			 */
			inline std::bitset<bit_width> getState() const {
				return this->state;
			}

			/**	\brief	Gets the SynchrotronComponentSetInsertEnd's input connections.
			 *
			 *	\return	std::set<SynchrotronComponentSetInsertEnd*>&
			 *      Returns a reference set to this SynchrotronComponentSetInsertEnd's inputs.
			 */
			const std::set<SynchrotronComponentSetInsertEnd*>& getIputs() const {
				return this->signalInput;
			}

			/**	\brief	Gets the SynchrotronComponentSetInsertEnd's output connections.
			 *
			 *	\return	std::set<SynchrotronComponentSetInsertEnd*>&
			 *      Returns a reference set to this SynchrotronComponentSetInsertEnd's outputs.
			 */
			const std::set<SynchrotronComponentSetInsertEnd*>& getOutputs() const {
				return this->slotOutput;
			}

			/**	\brief	Adds/Connects a new input to this SynchrotronComponentSetInsertEnd.
			 *
			 *	**Ensures both way connection will be made:**
			 *	This will have input added to its inputs and input will have this added to its outputs.
			 *
			 *	\param	input
			 *		The SynchrotronComponentSetInsertEnd to connect as input.
			 */
			void addInput(SynchrotronComponentSetInsertEnd& input) {
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
			void addInput(std::initializer_list<SynchrotronComponentSetInsertEnd*> inputList) {
				for(auto connection : inputList)
					this->addInput(*connection);
			}

			/**	\brief	Removes/Disconnects an input to this SynchrotronComponentSetInsertEnd.
			 *
			 *	**Ensures both way connection will be removed:**
			 *	This will have input removed from its inputs and input will have this removed from its outputs.
			 *
			 *	\param	input
			 *		The SynchrotronComponentSetInsertEnd to disconnect as input.
			 */
			void removeInput(SynchrotronComponentSetInsertEnd& input) {
				LockBlock lock(this);

				input.disconnectSlot(this);
			}

			/**	\brief	Adds/Connects a new output to this SynchrotronComponentSetInsertEnd.
			 *
			 *	**Ensures both way connection will be made:**
			 *	This will have output added to its outputs and output will have this added to its inputs.
			 *
			 *	\param	output
			 *		The SynchrotronComponentSetInsertEnd to connect as output.
			 */
			void addOutput(SynchrotronComponentSetInsertEnd& output) {
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
			void addOutput(std::initializer_list<SynchrotronComponentSetInsertEnd*> outputList) {
				for(auto connection : outputList)
					this->addOutput(*connection);
			}

			/**	\brief	Removes/Disconnects an output to this SynchrotronComponentSetInsertEnd.
			 *
			 *	**Ensures both way connection will be removed:**
			 *	This will have output removed from its output and output will have this removed from its inputs.
			 *
			 *	\param	output
			 *		The SynchrotronComponentSetInsertEnd to disconnect as output.
			 */
			void removeOutput(SynchrotronComponentSetInsertEnd& output) {
				LockBlock lock(this);

				this->disconnectSlot(&output);
			}

			/**	\brief	The tick() method will be called when one of this SynchrotronComponentSetInsertEnd's inputs issues an emit().
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
					this->state |= ((SynchrotronComponentSetInsertEnd*) connection)->getState();
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

#endif // SYNCHROTRONCOMPONENTSETINSERTEND_HPP
