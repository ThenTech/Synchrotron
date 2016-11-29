#ifndef SYNCHROTRONCOMPONENTVECTOR_HPP
#define SYNCHROTRONCOMPONENTVECTOR_HPP


#include <iostream> // For testing for now

#include "SynchrotronComponent.hpp"
#include <bitset>
#include <vector>
#include <algorithm>

namespace Synchrotron {

	/** \brief
	 *	SynchrotronComponent is the base for all components,
	 *	offering in and output connections to other SynchrotronComponent.
	 *
	 *	\param	bit_width
	 *		This template argument specifies the width of the internal bitset state.
	 */
	template <size_t bit_width>
	class SynchrotronComponentVector : public Mutex {
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
			std::vector<SynchrotronComponentVector*> slotOutput;

			/**	\brief
			 *	**Signals == inputs**
			 *
			 *		Receive tick()s from these subscriptions in signalInput.
			 */
			std::vector<SynchrotronComponentVector*> signalInput;

			/**	\brief	Connect a new slot s:
			 *		* Add s to this SynchrotronComponent's outputs.
			 *		* Add this to s's inputs.
			 *
			 *	\param	s
			 *		The SynchrotronComponent to connect.
			 */
			inline void connectSlot(SynchrotronComponentVector* s) {
				//LockBlock lock(this);

				this->slotOutput.push_back(s);
				s->signalInput.push_back(this);
			}

			/**	\brief	Disconnect a slot s:
			 *		* Remove s from this SynchrotronComponent's outputs.
			 *		* Remove this from s's inputs.
			 *
			 *	\param	s
			 *		The SynchrotronComponent to disconnect.
			 */
			inline void disconnectSlot(SynchrotronComponentVector* s) {
				//LockBlock lock(this);

				this->slotOutput.erase(std::remove(this->slotOutput.begin(), this->slotOutput.end(), s), this->slotOutput.end());
				s->signalInput.erase(std::remove(this->signalInput.begin(), this->signalInput.end(), s), this->signalInput.end());
			}

		public:
			/** \brief	Default constructor
			 *
			 *	\param	initial_value
			 *		The initial state of the internal bitset.
			 *	\param	bit_width
			 *		The size of the internal width of the bitset.
			 */
			SynchrotronComponentVector(size_t initial_value = 0) : state(initial_value) {}

			/**	\brief
			 *	Copy constructor
			 *	*	Duplicates signal subscriptions (inputs)
			 *	*	Optionally also duplicates slot connections (outputs)
			 *
			 *	\param	sc const
			 *		The other SynchrotronComponent to duplicate the connections from.
			 *	\param	duplicateAll_IO
			 *		Specifies whether to only copy inputs (false) or outputs as well (true).
			 */
			SynchrotronComponentVector(const SynchrotronComponentVector& sc, bool duplicateAll_IO = false) : SynchrotronComponentVector() {
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
			SynchrotronComponentVector(std::initializer_list<SynchrotronComponentVector*> inputList,
								 std::initializer_list<SynchrotronComponentVector*> outputList = {})
									: SynchrotronComponentVector() {
				this->addInput(inputList);
				this->addOutput(outputList);
			}

			/** \brief	Default destructor
			 *
			 *		When called, will disconnect all in and output connections to this SynchrotronComponent.
			 */
			~SynchrotronComponentVector() {
				LockBlock lock(this);

//				// Disconnect all Slots
//				for(auto& connection : this->slotOutput) {
//					//connection->signalInput.erase(this);
//					//delete connection; //?
//					connection->signalInput.erase(std::remove(connection->signalInput.begin(), connection->signalInput.end(), *this), connection->signalInput.end());
//				}

//				// Disconnect all Signals
//				for(auto &sender: this->signalInput) {
//					//sender->signalInput.erase(this);
//					sender->slotOutput.erase(std::remove(sender->signalInput.begin(), sender->signalInput.end(), *this), sender->signalInput.end());
//				}

				this->slotOutput.clear();
				this->signalInput.clear();
			}

			/**	\brief	Gets this SynchrotronComponent's bit width.
			 *
			 *	\return	size_t
			 *      Returns the bit width of the internal bitset.
			 */
			size_t getBitWidth() const {
				return bit_width;
			}

//			/* No real use since function cannot be called with different size SynchrotronComponents */
//			/* Maybe viable when SynchrotronComponent has different in and output sizes */
//			/*	\brief	Compare this bit width to that of other.
//             *
//             *	\param	other
//			 *		The other SynchrotronComponent to check.
//			 *
//             *	\return	bool
//             *      Returns whether the widths match.
//             */
//			inline bool hasSameWidth(SynchrotronComponent& other) {
//				return this->getBitWidth() == other.getBitWidth();
//			}

			/**	\brief	Gets this SynchrotronComponent's state.
			 *
			 *	\return	std::bitset<bit_width>
			 *      Returns the internal bitset.
			 */
			inline std::bitset<bit_width> getState() const {
				return this->state;
			}

			/**	\brief	Gets the SynchrotronComponent's input connections.
			 *
			 *	\return	std::set<SynchrotronComponent*>&
			 *      Returns a reference set to this SynchrotronComponent's inputs.
			 */
			const std::vector<SynchrotronComponentVector*>& getIputs() const {
				return this->signalInput;
			}

			/**	\brief	Gets the SynchrotronComponent's output connections.
			 *
			 *	\return	std::set<SynchrotronComponent*>&
			 *      Returns a reference set to this SynchrotronComponent's outputs.
			 */
			const std::vector<SynchrotronComponentVector*>& getOutputs() const {
				return this->slotOutput;
			}

			/**	\brief	Adds/Connects a new input to this SynchrotronComponent.
			 *
			 *	**Ensures both way connection will be made:**
			 *	This will have input added to its inputs and input will have this added to its outputs.
			 *
			 *	\param	input
			 *		The SynchrotronComponent to connect as input.
			 */
			void addInput(SynchrotronComponentVector& input) {
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
			void addInput(std::initializer_list<SynchrotronComponentVector*> inputList) {
				for(auto connection : inputList)
					this->addInput(*connection);
			}

			/**	\brief	Removes/Disconnects an input to this SynchrotronComponent.
			 *
			 *	**Ensures both way connection will be removed:**
			 *	This will have input removed from its inputs and input will have this removed from its outputs.
			 *
			 *	\param	input
			 *		The SynchrotronComponent to disconnect as input.
			 */
			void removeInput(SynchrotronComponentVector& input) {
				LockBlock lock(this);

				input.disconnectSlot(this);
			}

			/**	\brief	Adds/Connects a new output to this SynchrotronComponent.
			 *
			 *	**Ensures both way connection will be made:**
			 *	This will have output added to its outputs and output will have this added to its inputs.
			 *
			 *	\param	output
			 *		The SynchrotronComponent to connect as output.
			 */
			void addOutput(SynchrotronComponentVector& output) {
				LockBlock lock(this);

				// deprecated? //if (!this->hasSameWidth(*output)) return false;
				this->connectSlot(&output);
			}

			void addOutput(std::initializer_list<SynchrotronComponentVector*> outputList) {
				for(auto connection : outputList)
					this->addOutput(*connection);
			}

			/**	\brief	Removes/Disconnects an output to this SynchrotronComponent.
			 *
			 *	**Ensures both way connection will be removed:**
			 *	This will have output removed from its output and output will have this removed from its inputs.
			 *
			 *	\param	output
			 *		The SynchrotronComponent to disconnect as output.
			 */
			void removeOutput(SynchrotronComponentVector& output) {
				LockBlock lock(this);

				this->disconnectSlot(&output);
			}

			/**	\brief	The tick() method will be called when one of this SynchrotronComponent's inputs issues an emit().
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
					this->state |= ((SynchrotronComponentVector*) connection)->getState();
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

#endif // SYNCHROTRONCOMPONENTVECTOR_HPP
