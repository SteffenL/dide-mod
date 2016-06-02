#include <core/CoreVersion.h>
#include "CoreVersion_generated.h"

extern std::string CoreGetVersion()
{
	return CORE_VERSION_SCM_COMMIT;
}
