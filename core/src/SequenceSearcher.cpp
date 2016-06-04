#include <core/SequenceSearcher.h>
#include <cstring>
#include <stdexcept>
#include <cmath>

SequenceSearcher::SequenceSearcher(const char* bufferStart, const char* bufferEnd)
	: m_bufferStart(bufferStart), m_bufferEnd(bufferEnd)
{
}

bool SequenceSearcher::Search(
	const char* searchMask,
	uintptr_t* foundOffset)
{
	auto inputLength = ::strnlen_s(searchMask, MAX_MASK_LENGTH);
	if (!inputLength) {
		return false;
	}

	auto normalizedInputLength = static_cast<unsigned int>(floor((inputLength + 1) / 2.0)) * 2;
	if (inputLength != normalizedInputLength) {
		return false;
	}

	bool hasMatch = false;
	for (const char* absolutePos = m_bufferStart; absolutePos < m_bufferEnd; ++absolutePos) {
		const char* branchPos = absolutePos;
		unsigned int matchCount = 0;

		for (unsigned int i = 0; i < normalizedInputLength; ++i) {
			unsigned char bitshift = (i % 2) == 0 ? 4 : 0;

			if (searchMask[i] == '?') {
				// Wildcard matches by default
			}
			else {
				// Exact match
				char branchChar = (*branchPos >> bitshift) & 0xf;
				if (branchChar != hexDigitToDec(searchMask[i])) {
					// No match
					break;
				}
			}

			++matchCount;
			branchPos += i % 2;
		}

		// Not enough matches?
		if (matchCount < inputLength) {
			continue;
		}

		if (foundOffset) {
			*foundOffset = reinterpret_cast<uintptr_t>(absolutePos) - reinterpret_cast<uintptr_t>(m_bufferStart);
		}

		hasMatch = true;
		break;
	}

	return hasMatch;
}

unsigned char SequenceSearcher::hexDigitToDec(const char c) const
{
	if (c >= '0' && c <= '9') {
		return c - '0';
	}
	else if (c >= 'A' && c <= 'F') {
		return c - 'A' + 0xa;
	}
	else if (c >= 'a' && c <= 'f') {
		return c - 'a' + 0xa;
	}
	else {
		throw std::runtime_error("Invalid input");
	}
}
