#pragma once
/*******************************************************************************
 * Copyright 2011 See AUTHORS file.
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

#include <ecstasy/core/Entity.hpp>

namespace ecstasy {
	class ComponentBlueprint;

	/**
	 * Component factory interface. Used to construct {@link Component}s from {@link ComponentBlueprint}s.
	 */
	class ComponentFactory {
	public:
		/**
		 * Create a Component based on the blueprint and add it to the Entity.
		 *
		 * @param entity the Entity to add the Component to.
		 * @param blueprint the blueprint
		 * @return @a true on success.
		 */
		virtual bool assemble(Entity* entity, ComponentBlueprint& blueprint) = 0;
	};

	/**
	 * A template ComponentFactory implementation for simple components
	 * which don't need to read data from the blueprint.
	 */
	template<typename T>
	class SimpleComponentFactory : public ComponentFactory {
	public:
		/// Default constructor
		SimpleComponentFactory() {}
		SimpleComponentFactory(const SimpleComponentFactory&) = delete;

		bool assemble(Entity* entity, ComponentBlueprint& blueprint) override {
			return entity->emplace<T>();
		};
	};
}

#ifdef USING_ECSTASY
	using ecstasy::ComponentFactory;
	using ecstasy::SimpleComponentFactory;
#endif
