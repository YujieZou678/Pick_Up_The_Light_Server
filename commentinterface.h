#ifndef COMMENTINTERFACE_H
#define COMMENTINTERFACE_H

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class CommentInterface
{
public:
    CommentInterface();
    virtual ~CommentInterface() {}

    virtual json getInfo() = 0;
};

#endif // COMMENTINTERFACE_H
