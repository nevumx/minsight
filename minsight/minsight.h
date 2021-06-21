#ifndef minsight_h
#define minsight_h

#include <iostream>
#include <string>
#include <map>

// This is the macro which controls whether we use the debug new/delete, or the
// standard release new/delete
#ifdef DEBUG_MEMORY

// This is the #define which where we add/edit/remove the the possible memory
// types that can be allocated.
#define MEM_TYPES(MEM_TYPE) \
MEM_TYPE(Textures, 4096) \
MEM_TYPE(Animations, 2048) \
MEM_TYPE(Misc, 1024) \
MEM_TYPE(Default, 1024) \
// This line left intentionally blank.

enum class MemType : std::uint8_t
{
#define MEM_TYPE_ENUMS(Name, Size) Name,
	MEM_TYPES(MEM_TYPE_ENUMS)
#undef MEM_TYPE_ENUMS
};

#define DELETE_CG_CTORS(Type) \
private: \
Type& operator=(Type&&) = delete; \
Type(Type&& other) = delete; \
Type& operator=(const Type&) = delete;

class mInsight
{
private:
	struct MemInfo
	{
		const std::size_t TargetSize;
		std::size_t CurrentSize;
		std::string Name;

		MemInfo(const std::size_t newTargetSize, std::size_t newCurrentSize, const char* const newName)
		: TargetSize(newTargetSize), CurrentSize(newCurrentSize), Name(newName) {}

		MemInfo() : TargetSize(), CurrentSize(), Name() {}
		MemInfo(const MemInfo&) = default;
		DELETE_CG_CTORS(MemInfo)
	};

	struct Allocation
	{
		const MemType Type;
		const std::size_t Size;
		const std::string Function;
		const std::string File;
		const int Line;

		Allocation(const MemType newType, const std::size_t newSize, const char* const newFunction, const char* const newFile, const int newLine)
		: Type(newType), Size(newSize), Function(newFunction), File(newFile), Line(newLine) {}

		Allocation(const Allocation&) = default;
		DELETE_CG_CTORS(Allocation)
		Allocation() = delete;
	};

	static std::map<const MemType, MemInfo> MemInfos;
	static std::map<const void* const, const Allocation> MemAllocs;

public:
	static void* Alloc(MemType type, std::size_t size, const char* const function, const char* const file, const int line)
	{
		void* pointer = std::malloc(size);
		if (pointer != nullptr)
		{
			const auto insertion = MemAllocs.insert(std::make_pair(pointer, Allocation(type, size, function, file, line)));
			if (insertion.second)
			{
				if ((MemInfos[type].CurrentSize += size) > MemInfos[type].TargetSize)
				{
					std::cout << "Warning! Allocation in function \"" << function << "\" at line " << line << " of file \"" << file
					<< "\" has exceeded the Target Size of MemType " << MemInfos[type].Name << ". ("
					<<  MemInfos[type].CurrentSize << " / " <<  MemInfos[type].TargetSize << " bytes, "
					<< (float) MemInfos[type].CurrentSize / MemInfos[type].TargetSize * 100.0f << "%)" << std::endl << std::endl;
				}
			}
			else
			{
				std::cout << "Warning! malloc shomehow returning pointer still in use!" << std::endl << std::endl;
			}
		}
		return pointer;
	}

	static void Free(void* const pointer)
	{
		if (pointer != nullptr)
		{
			const auto element = MemAllocs.find(pointer);
			if (element != MemAllocs.end())
			{
				MemInfos[element->second.Type].CurrentSize -= element->second.Size;
				MemAllocs.erase(element);
			}
			else
			{
				std::cout << "Warning! Attempting to Free an invalid pointer!" << std::endl << std::endl;
			}
		}
		std::free(pointer);
	}

	static void PrintDebugInfo()
	{
		std::cout << "Memory Type Status:" << std::endl;
		for (std::map<const MemType, MemInfo>::const_iterator i = MemInfos.cbegin(); i != MemInfos.cend(); ++i)
		{
			std::cout << i->second.Name << ": " << i->second.CurrentSize << " / " << i->second.TargetSize
			<< " bytes (" << (float) i->second.CurrentSize / i->second.TargetSize * 100.0f << "%)" << std::endl;
		}
		std::cout << std::endl;
	}

	static void PrintTerminationInfo()
	{
		if (MemAllocs.empty())
		{
			std::cout << "Memory on Exit is OK." << std::endl << std::endl;
		}
		else
		{
			std::cout << "Warning: Unfreed allocations exist on exit; possible memory leaks:" << std::endl;
			for (std::map<const void* const, const Allocation>::const_iterator i = MemAllocs.cbegin(); i != MemAllocs.cend(); ++i)
			{
				std::cout << "MemType: " << MemInfos[i->second.Type].Name << ": " << i->second.Size << " bytes from function \"" << i->second.Function << "\" in file " << i->second.File << ":" << i->second.Line << "." << std::endl;
			}
			std::cout << std::endl;
			mInsight::PrintDebugInfo();
		}
	}
};

std::map<const MemType, mInsight::MemInfo> mInsight::MemInfos
{
#define MEM_TYPE_MAP(Name, Size) { MemType::Name, mInsight::MemInfo(Size, 0, #Name) },
	MEM_TYPES(MEM_TYPE_MAP)
#undef MEM_TYPE_MAP
};

std::map<const void* const, const mInsight::Allocation> mInsight::MemAllocs;

#define mNew(MemoryType, ObjType, ...) (new (mInsight::Alloc(MemType::MemoryType, sizeof(ObjType), __FUNCTION__, __FILE__, __LINE__)) ObjType(__VA_ARGS__))
#define mDelete(Pointer) (mInsight::Free(Pointer))
#define mNewArray(MemoryType, ObjType, ObjNum) (new (mInsight::Alloc(MemType::MemoryType, sizeof(ObjType) * (ObjNum), __FUNCTION__, __FILE__, __LINE__)) ObjType[ObjNum])
#define mDeleteArray(Pointer) mInsight::Free(Pointer)
#define PRINT_DEBUG_MEMORY_INFO mInsight::PrintDebugInfo();
#define PRINT_TERMINATION_MEMORY_INFO mInsight::PrintTerminationInfo();
#else
#define mNew(MemoryType, ObjType, ...) (new ObjType(__VA_ARGS__))
#define mDelete(Pointer) (delete Pointer)
#define mNewArray(MemoryType, ObjType, ObjNum) (new ObjType[ObjNum])
#define mDeleteArray(Pointer) (delete[] (Pointer))
#define PRINT_DEBUG_MEMORY_INFO
#define PRINT_TERMINATION_MEMORY_INFO
#endif

#endif /* minsight_h */
