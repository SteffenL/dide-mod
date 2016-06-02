#include <core/SequenceSearcher.h>
#include <cstring>

SequenceSearcher::SequenceSearcher(const char* bufferStart, const char* bufferEnd)
	: m_bufferStart(bufferStart), m_bufferEnd(bufferEnd)
{
}

bool SequenceSearcher::Search(
	const char* searchData,
	const char* searchMask,
	uintptr_t* foundOffset)
{
	const char* pbuf = m_bufferStart;
	const char* pdata = searchData;
	const char* pmask = searchMask;
	size_t dataSize = ::strnlen_s(pmask, MAX_MASK_LENGTH);
	if (!dataSize)
	{
		return false;
	}

	size_t matchCount = 0;
	bool isFullMatch = false;

	while (pbuf < (m_bufferEnd - dataSize))
	{
		bool charMatches = false;

		// Exact match
		if (*pmask == 'x')
		{
			// Character in buffer matches character in search data?
			if (*pbuf == *pdata)
			{
				charMatches = true;
			}
		}
		// Unknown, treat it as a match
		else if (*pmask == '?')
		{
			charMatches = true;
		}

		if (!charMatches)
		{
			// Didn't match, reset pointers
			
			// Reset the pointer back to the first match
			pbuf -= matchCount;
			// Advance to the next character in the buffer
			++pbuf;

			matchCount = 0;
			pdata = searchData;
			pmask = searchMask;
			continue;
		}

		++matchCount;

		// Advance to the next character
		++pbuf;

		// Full match?
		if (matchCount >= dataSize)
		{
			// Reset the pointer back to the first match
			pbuf -= matchCount;
			// Calculate the found offset
			*foundOffset = (pbuf - m_bufferStart);
			isFullMatch = true;
			break;
		}

		// Advance to the next character
		// Moved to the end to save a few CPU cycles
		++pdata;
		++pmask;
	}

	return isFullMatch;
}
