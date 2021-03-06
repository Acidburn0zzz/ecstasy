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
#include <ecstasy/systems/IteratingSystem.hpp>

#define NS_TEST_CASE(name) TEST_CASE("Family: " name)
namespace FamilyTests {
	struct ComponentA : public Component<ComponentA> {};
	struct ComponentB : public Component<ComponentB> {};
	struct ComponentC : public Component<ComponentC> {};
	struct ComponentD : public Component<ComponentD> {};
	struct ComponentE : public Component<ComponentE> {};
	struct ComponentF : public Component<ComponentF> {};

	class TestSystemA : public IteratingSystem<TestSystemA> {
	public:
		TestSystemA(std::string name) : IteratingSystem(Family::all<ComponentA>().get()) {}

	protected:
		void processEntity(Entity* e, float d) override {}
	};

	class TestSystemB : public IteratingSystem<TestSystemB> {
	public:
		TestSystemB (std::string name) : IteratingSystem(Family::all<ComponentB>().get()) {}

		void processEntity (Entity* e, float d) override {}
	};

	NS_TEST_CASE("sameFamily") {
		auto &family1 = Family::all<ComponentA>().get();
		auto &family2 = Family::all<ComponentA>().get();
		auto &family3 = Family::all<ComponentA, ComponentB>().get();
		auto &family4 = Family::all<ComponentA, ComponentB>().get();
		auto &family5 = Family::all<ComponentA, ComponentB, ComponentC>().get();
		auto &family6 = Family::all<ComponentA, ComponentB, ComponentC>().get();
		auto &family7 = Family::all<ComponentA, ComponentB>().one<ComponentC, ComponentD>()
			.exclude<ComponentE, ComponentF>().get();
		auto &family8 = Family::all<ComponentA, ComponentB>().one<ComponentC, ComponentD>()
			.exclude<ComponentE, ComponentF>().get();
		auto &family9 = Family::all().get();
		auto &family10 = Family::all().get();

		REQUIRE(family1 == family2);
		REQUIRE(family2 == family1);
		REQUIRE(family3 == family4);
		REQUIRE(family4 == family3);
		REQUIRE(family5 == family6);
		REQUIRE(family6 == family5);
		REQUIRE(family7 == family8);
		REQUIRE(family8 == family7);
		REQUIRE(family9 == family10);

		REQUIRE(family1.index == family2.index);
		REQUIRE(family3.index == family4.index);
		REQUIRE(family5.index == family6.index);
		REQUIRE(family7.index == family8.index);
		REQUIRE(family9.index == family10.index);
	}

	NS_TEST_CASE("differentFamily") {
		auto &family1 = Family::all<ComponentA>().get();
		auto &family2 = Family::all<ComponentB>().get();
		auto &family3 = Family::all<ComponentC>().get();
		auto &family4 = Family::all<ComponentA, ComponentB>().get();
		auto &family5 = Family::all<ComponentA, ComponentC>().get();
		auto &family6 = Family::all<ComponentB, ComponentA>().get();
		auto &family7 = Family::all<ComponentB, ComponentC>().get();
		auto &family8 = Family::all<ComponentC, ComponentA>().get();
		auto &family9 = Family::all<ComponentC, ComponentB>().get();
		auto &family10 = Family::all<ComponentA, ComponentB, ComponentC>().get();
		auto &family11 = Family::all<ComponentA, ComponentB>().one<ComponentC, ComponentD>()
			.exclude<ComponentE, ComponentF>().get();
		auto &family12 = Family::all<ComponentC, ComponentD>().one<ComponentE, ComponentF>()
			.exclude<ComponentA, ComponentB>().get();
		auto &family13 = Family::all().get();

		REQUIRE(family1 != family2);
		REQUIRE(family1 != family3);
		REQUIRE(family1 != family4);
		REQUIRE(family1 != family5);
		REQUIRE(family1 != family6);
		REQUIRE(family1 != family7);
		REQUIRE(family1 != family8);
		REQUIRE(family1 != family9);
		REQUIRE(family1 != family10);
		REQUIRE(family1 != family11);
		REQUIRE(family1 != family12);
		REQUIRE(family1 != family13);

		REQUIRE(family10 != family1);
		REQUIRE(family10 != family2);
		REQUIRE(family10 != family3);
		REQUIRE(family10 != family4);
		REQUIRE(family10 != family5);
		REQUIRE(family10 != family6);
		REQUIRE(family10 != family7);
		REQUIRE(family10 != family8);
		REQUIRE(family10 != family9);
		REQUIRE(family11 != family12);
		REQUIRE(family10 != family13);

		REQUIRE(family1.index != family2.index);
		REQUIRE(family1.index != family3.index);
		REQUIRE(family1.index != family4.index);
		REQUIRE(family1.index != family5.index);
		REQUIRE(family1.index != family6.index);
		REQUIRE(family1.index != family7.index);
		REQUIRE(family1.index != family8.index);
		REQUIRE(family1.index != family9.index);
		REQUIRE(family1.index != family10.index);
		REQUIRE(family11.index != family12.index);
		REQUIRE(family1.index != family13.index);
	}

	NS_TEST_CASE("familyEqualityFiltering") {
		auto &family1 = Family::all<ComponentA>().one<ComponentB>().exclude<ComponentC>().get();
		auto &family2 = Family::all<ComponentB>().one<ComponentC>().exclude<ComponentA>().get();
		auto &family3 = Family::all<ComponentC>().one<ComponentA>().exclude<ComponentB>().get();
		auto &family4 = Family::all<ComponentA>().one<ComponentB>().exclude<ComponentC>().get();
		auto &family5 = Family::all<ComponentB>().one<ComponentC>().exclude<ComponentA>().get();
		auto &family6 = Family::all<ComponentC>().one<ComponentA>().exclude<ComponentB>().get();

		REQUIRE(family1 == family4);
		REQUIRE(family2 == family5);
		REQUIRE(family3 == family6);
		REQUIRE(family1 != family2);
		REQUIRE(family1 != family3);
	}

	NS_TEST_CASE("entityMatch") {
		TEST_MEMORY_LEAK_START
		auto &family = Family::all<ComponentA, ComponentB>().get();

		Engine engine;
		Entity* entity = engine.createEntity();
		engine.addEntity(entity);
		entity->emplace<ComponentA>();
		entity->emplace<ComponentB>();

		REQUIRE(family.matches(entity));

		entity->emplace<ComponentC>();

		REQUIRE(family.matches(entity));
		TEST_MEMORY_LEAK_END
	}

	NS_TEST_CASE("entityMismatch") {
		TEST_MEMORY_LEAK_START
		auto &family = Family::all<ComponentA, ComponentC>().get();

		Engine engine;
		Entity* entity = engine.createEntity();
		engine.addEntity(entity);
		entity->emplace<ComponentA>();
		entity->emplace<ComponentB>();

		REQUIRE(!family.matches(entity));

		entity->remove<ComponentB>();

		REQUIRE(!family.matches(entity));
		TEST_MEMORY_LEAK_END
	}

	NS_TEST_CASE("entityMatchThenMismatch") {
		TEST_MEMORY_LEAK_START
		auto &family = Family::all<ComponentA, ComponentB>().get();

		Engine engine;
		Entity* entity = engine.createEntity();
		engine.addEntity(entity);
		entity->emplace<ComponentA>();
		entity->emplace<ComponentB>();

		REQUIRE(family.matches(entity));

		entity->remove<ComponentA>();

		REQUIRE(!family.matches(entity));
		TEST_MEMORY_LEAK_END
	}

	NS_TEST_CASE("entityMismatchThenMatch") {
		TEST_MEMORY_LEAK_START
		auto &family = Family::all<ComponentA, ComponentB>().get();

		Engine engine;
		Entity* entity = engine.createEntity();
		engine.addEntity(entity);
		entity->emplace<ComponentA>();
		entity->emplace<ComponentC>();

		REQUIRE(!family.matches(entity));

		entity->emplace<ComponentB>();

		REQUIRE(family.matches(entity));
		TEST_MEMORY_LEAK_END
	}

	NS_TEST_CASE("testEmptyFamily") {
		TEST_MEMORY_LEAK_START
		auto &family = Family::all().get();
		Engine engine;
		Entity* entity = engine.createEntity();
		engine.addEntity(entity);
		REQUIRE(family.matches(entity));
		TEST_MEMORY_LEAK_END
	}

	NS_TEST_CASE("familyFiltering") {
		TEST_MEMORY_LEAK_START
		auto &family1 = Family::all<ComponentA, ComponentB>().one<ComponentC, ComponentD>()
			.exclude<ComponentE, ComponentF>().get();

		auto &family2 = Family::all<ComponentC, ComponentD>().one<ComponentA, ComponentB>()
			.exclude<ComponentE, ComponentF>().get();

		Engine engine;
		Entity* entity = engine.createEntity();
		engine.addEntity(entity);

		REQUIRE(!family1.matches(entity));
		REQUIRE(!family2.matches(entity));

		entity->emplace<ComponentA>();
		entity->emplace<ComponentB>();

		REQUIRE(!family1.matches(entity));
		REQUIRE(!family2.matches(entity));

		entity->emplace<ComponentC>();

		REQUIRE(family1.matches(entity));
		REQUIRE(!family2.matches(entity));

		entity->emplace<ComponentD>();

		REQUIRE(family1.matches(entity));
		REQUIRE(family2.matches(entity));

		entity->emplace<ComponentE>();

		REQUIRE(!family1.matches(entity));
		REQUIRE(!family2.matches(entity));

		entity->remove<ComponentE>();

		REQUIRE(family1.matches(entity));
		REQUIRE(family2.matches(entity));

		entity->remove<ComponentA>();

		REQUIRE(!family1.matches(entity));
		REQUIRE(family2.matches(entity));
		TEST_MEMORY_LEAK_END
	}

	NS_TEST_CASE("matchWithEngine") {
		TEST_MEMORY_LEAK_START
		Engine engine;
		engine.emplaceSystem<TestSystemA>("A");
		engine.emplaceSystem<TestSystemA>("B");

		auto e = engine.createEntity();
		e->emplace<ComponentB>();
		e->emplace<ComponentA>();
		engine.addEntity(e);

		auto &f = Family::all<ComponentB>().exclude<ComponentA>().get();

		REQUIRE(!f.matches(e));
		TEST_MEMORY_LEAK_END
	}

	NS_TEST_CASE("matchWithEngineInverse") {
		TEST_MEMORY_LEAK_START
		Engine engine;

		engine.emplaceSystem<TestSystemA>("A");
		engine.emplaceSystem<TestSystemA>("B");

		auto e = engine.createEntity();
		e->emplace<ComponentB>();
		e->emplace<ComponentA>();
		engine.addEntity(e);

		auto &f = Family::all<ComponentA>().exclude<ComponentB>().get();

		REQUIRE(!f.matches(e));
		TEST_MEMORY_LEAK_END
	}

	NS_TEST_CASE("matchWithoutSystems") {
		TEST_MEMORY_LEAK_START
		Engine engine;

		auto e = engine.createEntity();
		e->emplace<ComponentB>();
		e->emplace<ComponentA>();
		engine.addEntity(e);

		auto &f = Family::all<ComponentB>().exclude<ComponentA>().get();

		REQUIRE(!f.matches(e));
		TEST_MEMORY_LEAK_END
	}

	NS_TEST_CASE("matchWithComplexBuilding") {
		TEST_MEMORY_LEAK_START
		auto &family = Family::all<ComponentB>().one<ComponentA>().exclude<ComponentC>().get();
		Engine engine;
		Entity* entity = engine.createEntity();
		engine.addEntity(entity);
		entity->emplace<ComponentA>();
		REQUIRE(!family.matches(entity));
		entity->emplace<ComponentB>();
		REQUIRE(family.matches(entity));
		entity->emplace<ComponentC>();
		REQUIRE(!family.matches(entity));
		TEST_MEMORY_LEAK_END
	}
}
