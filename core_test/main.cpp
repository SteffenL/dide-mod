#include <core/SequenceSearcher.h>
#include <string>
#include <iostream>
#include <stdexcept>
#include <sstream>

struct TestAssertError : public std::runtime_error
{
	TestAssertError(const std::string& file, unsigned int line)
		: runtime_error(formatError(file, line)) {}

private:
	std::string formatError(const std::string& file, unsigned int line)
	{
		std::stringstream ss;
		ss << "Test failed:\n  File: " << file << "\n  Line: " << line;
		return ss.str();
	}
};

#define assert(expression) if (!(expression)) throw TestAssertError(__FILE__, __LINE__)


void testSequenceSearcher()
{
	std::string buffer = "\x11\x22\x33\x44\x01\x02\x03\x04";
	SequenceSearcher ss(buffer.data(), buffer.data() + buffer.size());
	uintptr_t foundOffset;
	bool found = false;

	found = ss.Search("", &foundOffset);
	assert(!found);

	found = ss.Search("ff", &foundOffset);
	assert(!found);
	found = ss.Search("ffff", &foundOffset);
	assert(!found);
	found = ss.Search("ffffff", &foundOffset);
	assert(!found);
	found = ss.Search("ffffffff", &foundOffset);
	assert(!found);

	found = ss.Search("?f", &foundOffset);
	assert(!found);
	found = ss.Search("f?", &foundOffset);
	assert(!found);

	found = ss.Search("11", &foundOffset);
	assert(found && foundOffset == 0);
	found = ss.Search("22", &foundOffset);
	assert(found && foundOffset == 1);
	found = ss.Search("33", &foundOffset);
	assert(found && foundOffset == 2);
	found = ss.Search("44", &foundOffset);
	assert(found && foundOffset == 3);

	found = ss.Search("11223344", &foundOffset);
	assert(found && foundOffset == 0);

	found = ss.Search("??223344", &foundOffset);
	assert(found && foundOffset == 0);
	found = ss.Search("11??3344", &foundOffset);
	assert(found && foundOffset == 0);
	found = ss.Search("1122??44", &foundOffset);
	assert(found && foundOffset == 0);
	found = ss.Search("112233??", &foundOffset);
	assert(found && foundOffset == 0);

	found = ss.Search("?1223344", &foundOffset);
	assert(found && foundOffset == 0);
	found = ss.Search("1?223344", &foundOffset);
	assert(found && foundOffset == 0);
	found = ss.Search("11?23344", &foundOffset);
	assert(found && foundOffset == 0);
	found = ss.Search("112?3344", &foundOffset);
	assert(found && foundOffset == 0);
	found = ss.Search("1122?344", &foundOffset);
	assert(found && foundOffset == 0);
	found = ss.Search("11223?44", &foundOffset);
	assert(found && foundOffset == 0);
	found = ss.Search("112233?4", &foundOffset);
	assert(found && foundOffset == 0);
	found = ss.Search("1122334?", &foundOffset);
	assert(found && foundOffset == 0);

	found = ss.Search("f", &foundOffset);
	assert(!found);
	found = ss.Search("0", &foundOffset);
	assert(!found);
	found = ss.Search("1", &foundOffset);
	assert(!found);
	found = ss.Search("2", &foundOffset);
	assert(!found);
	found = ss.Search("3", &foundOffset);
	assert(!found);
	found = ss.Search("4", &foundOffset);
	assert(!found);
}

void runTests()
{
	testSequenceSearcher();
}

int main()
{
	try {
		runTests();
	}
	catch (const TestAssertError& ex) {
		std::cerr << ex.what() << std::endl;
		return 1;
	}
	catch (std::exception& ex) {
		std::cerr << "Unhandled exception: " << ex.what() << std::endl;
		return 1;
	}
	catch (...) {
		std::cerr << "Unhandled exception." << std::endl;
		return 1;
	}

	return 0;
}
