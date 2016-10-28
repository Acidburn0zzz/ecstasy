#pragma once

#include <memory>
#include <vector>
#include <map>
#include <ecstasy/utils/MemoryManager.hpp>

namespace ecstasy {
	//Fixme: in debug, mark bytes on creation, allocation, free?
	struct MemoryPage {
		uint32_t unitSize;
		uint8_t freeUnits = 64;
		char* memory;
		char* dataStart;
		char* dataEnd;
		uint64_t bitflags = 0xFFFFFFFFFFFFFFFF;
		uint64_t dataOffset = 0; // to be used for alignment

		MemoryPage(uint32_t unitSize);
		~MemoryPage();

		/// @return memory with enough size, or nullptr if no free memory is available
		void* allocate();
		/// @return true if memory belongs to this page
		bool owns(void* memory);
		/// @return false if memory already free
		void free(void* memory);
	};

	class MemoryPageManager {
	private:
		uint32_t unitSize;
		uint32_t allocationCount = 0;
		std::vector<std::unique_ptr<MemoryPage>> pages;
		std::vector<MemoryPage*> freePages;

	public:
		MemoryPageManager(uint32_t unitSize) : unitSize(unitSize) {}
		MemoryPageManager(const MemoryPageManager &) = delete;
		~MemoryPageManager() {}

		uint32_t getAllocationCount() const {
			return allocationCount;
		}

		void* allocate();
		void free(void* memory);

		void reduceMemory();
	};

	class DefaultMemoryManager : public MemoryManager {
	private:
		std::map<uint32_t, std::unique_ptr<MemoryPageManager>> managers;

	public:
		DefaultMemoryManager() {}
		DefaultMemoryManager(const DefaultMemoryManager &) = delete;
		~DefaultMemoryManager() {}

		void* allocate(uint32_t size) override;
		void free(uint32_t size, void* memory) override;

		void reduceMemory() override;

		uint32_t getAllocationCount() const override;
	};
}
