#ifndef __SequenceSearcher__
#define __SequenceSearcher__

#include <cstdint>

class SequenceSearcher
{
	static const int MAX_MASK_LENGTH = 255;

private:
	const char* m_bufferStart;
	const char* m_bufferEnd;

public:
	SequenceSearcher(const char* bufferStart, const char* bufferEnd);

	bool Search(
		const char* searchMask,
		uintptr_t* foundOffset = nullptr);

private:
	unsigned char hexDigitToDec(const char c) const;
};

#endif // __SequenceSearcher__
