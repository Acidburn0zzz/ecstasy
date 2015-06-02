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
#include <ecstasy/core/ComponentOperations.h>
#include <ecstasy/core/Entity.h>
#include <ecstasy/core/Engine.h>
#include <ecstasy/core/Component.h>

namespace ECS {
	void ComponentOperation::makeAdd(Entity *entity, ComponentBase *component) {
		this->type = Type::Add;
		this->entity = entity;
		this->component = component;
		this->componentType = 0;
	}

	void ComponentOperation::makeRemove(Entity *entity, ComponentType componentType) {
		this->type = Type::Remove;
		this->entity = entity;
		this->component = nullptr;
		this->componentType = componentType;
	}

	void ComponentOperation::reset() {
		entity = nullptr;
		component = nullptr;
	}
	
	void ComponentOperationHandler::add(Entity *entity, ComponentBase *component) {
		if (engine.updating) {
			ComponentOperation *operation = engine.componentOperationsPool.obtain();
			operation->makeAdd(entity, component);
			engine.componentOperations.push_back(operation);
		}
		else {
			entity->addInternal(component);
		}
	}

	void ComponentOperationHandler::remove(Entity *entity, ComponentType componentType) {
		if (engine.updating) {
			ComponentOperation *operation = engine.componentOperationsPool.obtain();
			operation->makeRemove(entity, componentType);
			engine.componentOperations.push_back(operation);
		}
		else {
			entity->removeInternal(componentType);
		}
	}
}