#include "Directive.hpp"
#include "Utils.hpp"


Directive::Directive(std::string directiveName) : _directiveName(directiveName), _directiveValues()
{}

void    Directive::addDirectiveValue(std::string directiveValue){
        if (_directiveName == LISTEN || _directiveName == MAXBODYSIZE || _directiveName == ERRORFILE) // afterwards error_files should be checked if it respects the required format
    {
        try
        {
            int intValue = stringToInt(directiveValue);
            if (intValue < 0)
            {
                throw std::runtime_error("Port number should be a positive integer!");
            }
            // else
            // {
            //     _directiveValue._type = INT;
            // }
            directiveValueUnion u;
            u._intValue = intValue;
            _directiveValues.push_back(u);
            _type = INT;
        }
        catch (const std::exception &e)
        {
            // std::cout << "TEST" <<std::endl;                                                           // specify the exception type
            throw; // re-throw the exception with a modified message
        }
    }
    else
    {
        // _directiveValue._type = STRING;
        directiveValueUnion u;
        std::copy(directiveValue.begin(), directiveValue.begin() + DIRECTIVE_NAME_LENGTH, u._stringValue);
        _directiveValues.push_back(u);
        _type = STRING;
    }
}

std::vector<directiveValueUnion> const &	Directive::getDirectiveValues() const {
    return(_directiveValues);
}

t_type  const &Directive::getType() const {
    return(_type);
}

std::string const   &Directive::getDirectiveName() const {
    return (_directiveName);
}

std::ostream &operator<<(std::ostream &o, Directive const &i)
{
    typedef std::vector<directiveValueUnion> dir_vec;
    dir_vec             values = i.getDirectiveValues();
    dir_vec::iterator   valuesIter;
    if (i.getType() == INT){
        for (valuesIter = values.begin(); valuesIter != values.end(); valuesIter++) {
            o << valuesIter->_intValue;
        }
    }
    else {
        for (valuesIter = values.begin(); valuesIter != values.end(); valuesIter++) {
            o << valuesIter->_intValue;
        }
    }
	return o;
}
