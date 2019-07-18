#include "processor_types.hpp"

REGISTER,
    IMMEDIATE,
    MEMORY,
    HALT,
    HYBRID,

Entry::Entry(string raw)
{  
    switch (raw.at(0))
    {
        case '0':
            control_type = REGISTER;
            break;
        case '1':
            control_type = IMMEDIATE;
            break;
        default:
            control_type = M;
            break;
    }
    for (int i = 0; i < 4; i++)
    {
        org_line[i] = raw.substr(2 * i, 2);
    }
}

string Entry::getOppCode(void)
{
    return org_line[0];
}

string Entry::getRegister(void)
{
    return org_line[1];
}

string Entry::getRemaining(void)
{
    switch (control_type)
    {
        case R:
            return org_line[2];
        case I:
            return org_line[2] + org_line[3];
        default:
            return "";   
    }
}

EntryType Entry::getType(void)
{
    return control_type;
}