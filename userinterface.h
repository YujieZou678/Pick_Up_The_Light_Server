#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <string>
using std::string;
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class UserInterface
{
public:
    explicit UserInterface(const string &id);
    virtual ~UserInterface() {}

    virtual json getInfo()  = 0;

    string id() const;

private:
    string m_id;
};

#endif // USERINTERFACE_H
