/**
*	Synchronous component with combined Signals and Slots functionality.
* 		Loosely based on https://github.com/winglot/Signals
*/
#ifndef SYNCHROTRONCOMPONENT_HPP
#define SYNCHROTRONCOMPONENT_HPP

#include <iostream> // For testing for now

#include <bitset>
//#include <forward_list> // faster iteration?  TODO run test!!
#include <set>
#include <mutex>

namespace Synchrotron {

	class Mutex {
		private:
			std::mutex m_mutex;
		public:
			Mutex() 				{}
			Mutex(const Mutex&) 	{}
			virtual ~Mutex() 		{}
			virtual void lock()		{ m_mutex.lock(); }
			virtual void unlock()	{ m_mutex.unlock(); }
	};

	/**
	*	Creating a new LockBlock(this) locks the current thread,
	*	while leaving the scope conveniently unlocks the thread.
	*/
	class LockBlock {
		public:
			Mutex *m_mutex;
			LockBlock(Mutex *mtx)
				: m_mutex(mtx)		{ m_mutex->lock(); }
			~LockBlock()			{ m_mutex->unlock(); }
	};

	template <size_t bit_width>
	class SynchrotronComponent : public Mutex {
		private:
			/**
			 *	The current internal state of bits in this component (default output).
			 */
			std::bitset<bit_width> state;

			/**
			 *	Slots == outputs
			 *		Emit this.signal to subscribers in slotOutput.
			 */
			std::set<SynchrotronComponent*> slotOutput;

			/**
			 *	Signals == inputs
			 *		Receive tick()s from these subscriptions in signalInput.
			 */
			std::set<SynchrotronComponent*> signalInput;

			/***************************************************************
			 *	SIGNAL METHODS
			 ***************************************************************/
			inline void connectSlot(SynchrotronComponent* s) {
				//LockBlock lock(this);

				this->slotOutput.insert(s);
				s->signal_connect(this);
			}

			inline void disconnectSlot(SynchrotronComponent* s) {
				//LockBlock lock(this);

				this->slotOutput.erase(s);
				s->signal_disconnect(this);
			}

			inline void slot_disconnect(SynchrotronComponent* s) {
				//LockBlock lock(this);

				this->slotOutput.erase(s);
			}

			inline void slot_duplicate(SynchrotronComponent* s) {
				//LockBlock lock(this);

				this->slotOutput.insert(s);
			}

			void disconnect_all_slots() {
				//LockBlock lock(this);

				for(auto& connection : this->slotOutput) {
					connection->signal_disconnect(this);
					//delete connection; //?
				}

				//this->slots.erase(this->slots.begin(), this->slots.end());
				this->slotOutput.clear();
			}
			/***************************************************************/

			/***************************************************************
			 *	SLOT METHODS
			 ***************************************************************/
			inline void signal_connect(SynchrotronComponent* sender) {
				//LockBlock lock(this);

				this->signalInput.insert(sender);
			}

			inline void signal_disconnect(SynchrotronComponent* sender) {
				//LockBlock lock(this);

				this->signalInput.erase(sender);
			}


			void disconnect_all_signals() {
				//LockBlock lock(this);

				for(auto &sender: this->signalInput) {
					sender->slot_disconnect(this);
				}

				//this->connections.erase(this->connections.begin(), this->connections.end());
				this->signalInput.clear(); // ?
			}
			/***************************************************************/

		public:
			/**
			 *	Default constructor
			 */
			SynchrotronComponent(size_t initial_value = 0) : state(initial_value) {}

			/**
			 *	Copy constructor
			 *		Duplicates signal subscriptions (inputs)
			 *		Optionally also duplicates slot connections (outputs)
			 */
			SynchrotronComponent(const SynchrotronComponent& sc, bool duplicateAll_IO = false) : SynchrotronComponent() {
				LockBlock lock(this);

				// Copy subscribers
				for(auto& connection : sc.slotOutput) {
					connection->signal_connect(this);
					this->slotOutput.insert(connection);
				}

				if (duplicateAll_IO) {
					// Copy subscriptions
					for(auto& sender : sc.signalInput) {
						sender->slot_duplicate(this);
						this->signalInput.insert(sender);
					}
				}
			}

			~SynchrotronComponent() {
				LockBlock lock(this);
				this->disconnect_all_slots();
				this->disconnect_all_signals();
			}

			inline size_t getBitWidth() {
				return bit_width;
			}

			inline bool hasSameWidth(SynchrotronComponent& other) {
				return this->getBitWidth() == other.getBitWidth();
			}

			inline std::bitset<bit_width> getState() const {
				return this->state;
			}

//			std::set<SynchrotronComponent*>& getIputs() const {
//				return this->signalInput;
//			}

//			std::set<SynchrotronComponent*>& getOutputs() const {
//				return this->slotOutput;
//			}

			bool addInput(SynchrotronComponent *input) {
				LockBlock lock(this);

				if (!this->hasSameWidth(*input))
					return false;
				input->connectSlot(this);
				return true;
			}

			void removeInput(SynchrotronComponent *input) {
				LockBlock lock(this);

				input->disconnectSlot(this);
			}

			bool addOutput(SynchrotronComponent *output) {
				LockBlock lock(this);

				if (!this->hasSameWidth(*output))
					return false;
				this->connectSlot(output);
				return true;
			}

			void removeOutput(SynchrotronComponent *output) {
				LockBlock lock(this);

				this->disconnectSlot(output);
			}

			virtual inline void tick() {
				//LockBlock lock(this);

				std::cout << "Ticked\n";
				for(auto& connection : this->signalInput) {
					std::cout << "State = " << ((SynchrotronComponent*) connection)->getState() << "\n";
				}

				//this->emit(); // Directly emit changes to subscribers?
			}

			/**
			 *	Emit this signal and execute all subscribed slots
			 */
			virtual inline void emit() {
				//LockBlock lock(this);

				for(auto& connection : this->slotOutput) {
					connection->tick();
				}
				std::cout << "Emitted\n";
			}
	};

}


#endif // SYNCHROTRONCOMPONENT_HPP
