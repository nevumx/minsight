# minsight
`minsight` is an in-line memory debugging to tool for C++. It's just one header, `minsight.h`, and needs the following integration steps:

1. `#include "minsight.h"` in some place that is accessible to code that will need the functionality of: `new`, `new[]`, `delete`, and `delete[]`, as well as the below macros...
2. Replace all instances of `new`, `new[]`, `delete`, and `delete[]` with their debuggable versions: `mNew`, `mNewArray`, `mDelete`, and `mDeleteArray` respectively.
3. Add the `PRINT_DEBUG_MEMORY_INFO` macro at any point in the program where you wish to debug the current memory at that point in the program.
4. Add the `PRINT_TERMINATION_MEMORY_INFO` macro just before your program ends. (Or at the point that you think all the dynamically allocated memory should be deallocated)
5. In debug builds, (or whenever you want memory to be debugged), make sure to define the `DEBUG_MEMORY` preprocessor macro. (It doesn't need a value, it just needs to be defined)
6. In release builds, (or whenever you do not want your memory to be debugged), make sure that the `DEBUG_MEMORY` preprocessor macro is _not_ defined.
7. Edit the `MEM_TYPES` macro in `minsight.h` to add your possible memory types and budgets.
8. Edit all instances of `mNew`, `mNewArray`, `mDelete`, and `mDeleteArray` to take a memory type as their first argument.

Now, when running your program, you will be able to see where and how your memory is being allocated, and what category it is in within your memory budget.

For example, given the following memory types in `minsight.h`:
```
#define MEM_TYPES(MEM_TYPE) \
MEM_TYPE(Textures, 4096) \
MEM_TYPE(Animations, 2048) \
MEM_TYPE(Misc, 1024) \
MEM_TYPE(Default, 1024) \
```

And the following `main.cpp` file:
```
#include "minsight.h"

struct TestStruct
{
	int a;
	float b;
	double c;
	std::string d;
	
	TestStruct() {}
	TestStruct(int a, float b, double c, std::string d)
		: a(a), b(b), c(c), d(d) {}
};

int main(int argc, const char* argv[])
{
	int* temp = mNewArray(Textures, int, 40);
	
	PRINT_DEBUG_MEMORY_INFO
	
	mDeleteArray(temp);
	
	PRINT_DEBUG_MEMORY_INFO
	
	mNew(Default, TestStruct);
	TestStruct* temp3 = mNew(Misc, TestStruct, 2, 2.0f, 2.0, "2.0");
	mNewArray(Misc, TestStruct, 27);
	
	mDelete(temp3);
	
	PRINT_TERMINATION_MEMORY_INFO
	return 0;
}
```

You can expect the following output:
```
Memory Type Status:
Textures: 160 / 4096 bytes (3.90625%)
Animations: 0 / 2048 bytes (0%)
Misc: 0 / 1024 bytes (0%)
Default: 0 / 1024 bytes (0%)

Memory Type Status:
Textures: 0 / 4096 bytes (0%)
Animations: 0 / 2048 bytes (0%)
Misc: 0 / 1024 bytes (0%)
Default: 0 / 1024 bytes (0%)

Warning! Allocation in function "main" at line 27 of file "/Users/NEVUM_X/Desktop/minsight/minsight/main.cpp" has exceeded the Target Size of MemType Misc. (1120 / 1024 bytes, 109.375%)

Warning: Unfreed allocations exist on exit; possible memory leaks:
MemType: Default: 40 bytes from function "main" in file /Users/NEVUM_X/Desktop/minsight/minsight/main.cpp:25.
MemType: Misc: 1080 bytes from function "main" in file /Users/NEVUM_X/Desktop/minsight/minsight/main.cpp:27.

Memory Type Status:
Textures: 0 / 4096 bytes (0%)
Animations: 0 / 2048 bytes (0%)
Misc: 1080 / 1024 bytes (105.469%)
Default: 40 / 1024 bytes (3.90625%)

Program ended with exit code: 0
```

Pros:
- Support for replacing `new`, `new[]`, `delete`, and `delete[]` with debuggable versions: `mNew`, `mNewArray`, `mDelete`, and `mDeleteArray` respectively.
- Support for adding categories of memories that need to be debugged, _with_ budgets that will trigger a memory warning if exceeded.

Cons / Potential TODOS:
- No placement `new` support.
- No `malloc(...)`/`calloc(...)`/`realloc(...)`/`free(...)` support.
