/*******************************************************************************
 * Copyright 2015 See AUTHORS file.
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
#include "../TestBase.hpp"
#include <ecstasy/systems/SortedIteratingSystem.hpp>
#include <deque>

#define NS_TEST_CASE(name) TEST_CASE("SortedIteratingSystem: " name)
namespace SortedIteratingSystemTests {
	float deltaTime = 0.16f;

	struct ComponentB : public Component<ComponentB> {};
	struct ComponentC : public Component<ComponentC> {};

	struct OrderComponent : public Component<OrderComponent> {
		std::string name;
		int zLayer;
		OrderComponent(std::string name, int zLayer) : name(name), zLayer(zLayer) {}
	};

	struct SpyComponent : public Component<SpyComponent> {
		int updates = 0;
	};

	struct IndexComponent : public Component<IndexComponent> {
		int index;
		IndexComponent(int index) : index(index) {}
	};

	bool comparator(Entity* a, Entity* b) {
		auto ac = a->get<OrderComponent>();
		auto bc = b->get<OrderComponent>();
		return ac->zLayer < bc->zLayer;
	}

	struct Less {
		bool operator () (Entity* a, Entity* b) {
			auto ac = a->get<OrderComponent>();
			auto bc = b->get<OrderComponent>();
			return ac->zLayer < bc->zLayer;
		}
	};

	class SortedIteratingSystemMock : public SortedIteratingSystem<SortedIteratingSystemMock, Less> {
	public:
		std::deque<std::string> expectedNames;

		SortedIteratingSystemMock(const Family &family) : SortedIteratingSystem(family, Less()) {}

		void update(float deltaTime) override {
			SortedIteratingSystem::update(deltaTime);
			REQUIRE(expectedNames.empty());
		}

		void processEntity(Entity* entity, float deltaTime) override {
			auto component = entity->get<OrderComponent>();
			REQUIRE(component);
			REQUIRE(!expectedNames.empty());
			REQUIRE(expectedNames.front() == component->name);
			expectedNames.pop_front();
		}
	};

	class IteratingComponentRemovalSystem : public SortedIteratingSystem<IteratingComponentRemovalSystem, Less> {
	public:
		IteratingComponentRemovalSystem()
			:SortedIteratingSystem(Family::all<SpyComponent, IndexComponent>().get(), Less()) {
		}

		void processEntity (Entity* entity, float deltaTime) override {
			int index = entity->get<IndexComponent>()->index;
			if (index % 2 == 0) {
				entity->remove<SpyComponent>();
				entity->remove<IndexComponent>();
			} else {
				entity->get<SpyComponent>()->updates++;
			}
		}

	};

class IteratingRemovalSystem : public SortedIteratingSystem<IteratingRemovalSystem, Less> {

	public:
		Engine* engine;
		IteratingRemovalSystem()
			: SortedIteratingSystem(Family::all<SpyComponent, IndexComponent>().get(), Less()) {}

		void addedToEngine(Engine* engine) override {
			SortedIteratingSystem::addedToEngine(engine);
			this->engine = engine;
		}

		void processEntity(Entity* entity, float deltaTime) override {
			int index = entity->get<IndexComponent>()->index;
			if (index % 2 == 0)
				engine->removeEntity(entity);
			else
				entity->get<SpyComponent>()->updates++;
		}
	};

	NS_TEST_CASE("shouldIterateSortedEntitiesWithCorrectFamily") {
		TEST_MEMORY_LEAK_START
		Engine engine;

		auto &family = Family::all<OrderComponent, ComponentB>().get();
		auto system = engine.emplaceSystem<SortedIteratingSystemMock>(family);
		Entity* e = engine.createEntity();
		engine.addEntity(e);

		// When entity has OrderComponent
	 	e->emplace<OrderComponent>("A", 0);
		engine.update(deltaTime);

		// When entity has OrderComponent and ComponentB
		e->emplace<ComponentB>();
		system->expectedNames.push_back("A");
		engine.update(deltaTime);

		// When entity has OrderComponent, ComponentB and ComponentC
		e->emplace<ComponentC>();
		system->expectedNames.push_back("A");
		engine.update(deltaTime);

		// When entity has ComponentB and ComponentC
		e->remove<OrderComponent>();
		engine.update(deltaTime);
		TEST_MEMORY_LEAK_END
	}

	NS_TEST_CASE("entityRemovalWhileSortedIterating") {
		TEST_MEMORY_LEAK_START
		Engine engine;
		auto entities = engine.getEntitiesFor(Family::all<SpyComponent, IndexComponent>().get());

		engine.emplaceSystem<IteratingRemovalSystem>();

		int numEntities = 10;

		for (int i = 0; i < numEntities; ++i) {
			auto e = engine.createEntity();
			e->emplace<SpyComponent>();
			e->emplace<OrderComponent>(std::to_string(i), i);
			e->emplace<IndexComponent>(i + 1);

			engine.addEntity(e);
		}

		engine.update(deltaTime);

		REQUIRE((numEntities / 2) == entities->size());

		for (auto e: *entities) {
			REQUIRE(1 == e->get<SpyComponent>()->updates);
		}
		TEST_MEMORY_LEAK_END
	}

	NS_TEST_CASE("componentRemovalWhileSortedIterating") {
		TEST_MEMORY_LEAK_START
		Engine engine;
		auto entities = engine.getEntitiesFor(Family::all<SpyComponent, IndexComponent>().get());

		engine.emplaceSystem<IteratingComponentRemovalSystem>();

		int numEntities = 10;

		for (int i = 0; i < numEntities; ++i) {
			auto e = engine.createEntity();
			e->emplace<SpyComponent>();
			e->emplace<OrderComponent>(std::to_string(i), i);
			e->emplace<IndexComponent>(i + 1);

			engine.addEntity(e);
		}

		engine.update(deltaTime);

		REQUIRE((numEntities / 2) == entities->size());

		for (auto e: *entities) {
			REQUIRE(1 == e->get<SpyComponent>()->updates);
		}
		TEST_MEMORY_LEAK_END
	}

	Entity* createOrderEntity(std::string name, int zLayer, Engine &engine) {
		auto e = engine.createEntity();
		e->emplace<OrderComponent>(name, zLayer);
		return e;
	}

	NS_TEST_CASE("entityOrder") {
		TEST_MEMORY_LEAK_START
		Engine engine;

		auto &family = Family::all<OrderComponent>().get();
		auto system = engine.emplaceSystem<SortedIteratingSystemMock>(family);

		auto a = createOrderEntity("A", 0, engine);
		auto b = createOrderEntity("B", 1, engine);
		auto c = createOrderEntity("C", 3, engine);
		auto d = createOrderEntity("D", 2, engine);

		engine.addEntity(a);
		engine.addEntity(b);
		engine.addEntity(c);
		system->expectedNames.push_back("A");
		system->expectedNames.push_back("B");
		system->expectedNames.push_back("C");
		engine.update(0);

		engine.addEntity(d);
		system->expectedNames.push_back("A");
		system->expectedNames.push_back("B");
		system->expectedNames.push_back("D");
		system->expectedNames.push_back("C");
		engine.update(0);

		a->get<OrderComponent>()->zLayer = 3;
		b->get<OrderComponent>()->zLayer = 2;
		c->get<OrderComponent>()->zLayer = 1;
		d->get<OrderComponent>()->zLayer = 0;
		system->forceSort();
		system->expectedNames.push_back("D");
		system->expectedNames.push_back("C");
		system->expectedNames.push_back("B");
		system->expectedNames.push_back("A");
		engine.update(0);
		TEST_MEMORY_LEAK_END
	}
}
