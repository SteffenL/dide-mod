#pragma once

class ModBase {
public:
    virtual ~ModBase() = default;
    virtual void run() = 0;
};
