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
