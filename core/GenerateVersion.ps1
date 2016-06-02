$ErrorActionPreference = "Stop"

$scmCommit = &git rev-parse HEAD
$content = "#pragma once`n#define CORE_VERSION_SCM_COMMIT ""${scmCommit}""`n"
$content | Set-Content -Path "src/CoreVersion_generated.h"
