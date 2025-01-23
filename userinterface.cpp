#include "userinterface.h"

UserInterface::UserInterface(const string &id) :
    m_id(id)
{
}

string UserInterface::id() const
{
    return m_id;
}
