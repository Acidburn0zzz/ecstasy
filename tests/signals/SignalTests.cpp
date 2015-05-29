/*******************************************************************************
 * Copyright 2014 See AUTHORS file.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************************/
#include "../TestBase.h"

namespace SignalTests {
	class Dummy {

	};

	class ListenerMock : public Receiver < Dummy * > {
	public:
		int count = 0;

		void receive(Signal<Dummy *> &signal, Dummy *object) override {
			++count;

			REQUIRE(object != nullptr);
		}
	};

	class RemoveWhileDispatchListenerMock : public Receiver < Dummy * > {
	public:
		int count = 0;

		void receive(Signal<Dummy *> &signal, Dummy *object) override {
			++count;
			signal.remove(this);
		}
	};

	TEST_CASE("Add Listener and Dispatch") {
		Dummy dummy;
		Signal<Dummy *> signal;
		ListenerMock listener;
		signal.add(&listener);

		for (int i = 0; i < 10; ++i) {
			REQUIRE(i == listener.count);
			signal.dispatch(&dummy);
			REQUIRE((i + 1) == listener.count);
		}
	}

	TEST_CASE("Add Listeners and Dispatch") {
		Dummy dummy;
		Signal<Dummy *> signal;
		Allocator<ListenerMock> listeners;

		int numListeners = 10;

		for (int i = 0; i < numListeners; i++) {
			signal.add(listeners.create());
		}

		int numDispatchs = 10;

		for (int i = 0; i < numDispatchs; ++i) {
			for (auto listener : listeners.values) {
				REQUIRE(i == listener->count);
			}

			signal.dispatch(&dummy);

			for (auto listener : listeners.values) {
				REQUIRE((i + 1) == listener->count);
			}
		}
	}

	TEST_CASE("Add Listener Dispatch and Remove") {
		Dummy dummy;
		Signal<Dummy *> signal;
		ListenerMock listenerA;
		ListenerMock listenerB;

		signal.add(&listenerA);
		signal.add(&listenerB);

		int numDispatchs = 5;

		for (int i = 0; i < numDispatchs; ++i) {
			REQUIRE(i == listenerA.count);
			REQUIRE(i == listenerB.count);

			signal.dispatch(&dummy);

			REQUIRE((i + 1) == listenerA.count);
			REQUIRE((i + 1) == listenerB.count);
		}

		signal.remove(&listenerB);

		for (int i = 0; i < numDispatchs; ++i) {
			REQUIRE((i + numDispatchs) == listenerA.count);
			REQUIRE(numDispatchs == listenerB.count);

			signal.dispatch(&dummy);

			REQUIRE((i + 1 + numDispatchs) == listenerA.count);
			REQUIRE(numDispatchs == listenerB.count);
		}
	}

	TEST_CASE("Remove while dispatch") {
		Dummy dummy;
		Signal<Dummy *> signal;
		RemoveWhileDispatchListenerMock listenerA;
		ListenerMock listenerB;

		signal.add(&listenerA);
		signal.add(&listenerB);

		signal.dispatch(&dummy);

		REQUIRE(1 == listenerA.count);
		REQUIRE(1 == listenerB.count);
	}

	TEST_CASE("Remove all") {
		Dummy dummy;
		Signal<Dummy *> signal;

		ListenerMock listenerA;
		ListenerMock listenerB;

		signal.add(&listenerA);
		signal.add(&listenerB);

		signal.dispatch(&dummy);

		REQUIRE(1 == listenerA.count);
		REQUIRE(1 == listenerB.count);

		signal.removeAll();

		signal.dispatch(&dummy);

		REQUIRE(1 == listenerA.count);
		REQUIRE(1 == listenerB.count);
	}
}