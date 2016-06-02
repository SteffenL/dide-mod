#include <core/StandardPaths.h>
#include <nowide/convert.hpp>

#include <vector>

#include <Windows.h>
#include <Shlwapi.h>


std::string StandardPaths::GetExecutablePath() {
    std::vector<wchar_t> buffer;
    unsigned int bufferLength = MAX_PATH;
    buffer.resize(bufferLength);

    bool isOk = false;

    do {
        auto requiredLength = ::GetModuleFileNameW(NULL, &buffer[0], static_cast<DWORD>(buffer.size()));

        // Failed?
        /*if (requiredLength == 0) {
            throw WindowsException(::GetLastError());
        }*/

        // If the returned module path was truncated, we need a larger buffer
        if (requiredLength >= bufferLength) {
            // Increase buffer length
            bufferLength <<= 1;
            buffer.resize(bufferLength);
        }

        isOk = true;
    } while (!isOk);

    std::string path;
    path = nowide::narrow(&buffer[0]);
    
    return path;
}

std::string StandardPaths::GetExecutableDir()
{
	auto exePathW = nowide::widen(GetExecutablePath());
	std::vector<wchar_t> buffer(exePathW.begin(), exePathW.end());
	::PathRemoveFileSpecW(buffer.data());
	return nowide::narrow(buffer.data());
}
