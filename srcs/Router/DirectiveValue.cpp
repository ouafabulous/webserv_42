#include <DirectiveValue.hpp>
#include <Utils.hpp>

DirectiveValue::DirectiveValue(std::string directiveName, std::string directiveValue)
{
    if (directiveName == "listen")
    {
        try
        {
            _directiveValue._intValue = stringToInt(directiveValue);
            if (_directiveValue._intValue < 0)
            {
                throw std::runtime_error("Port number should be a positive integer!");
            }
            else
            {
                _directiveValue._type = INT;
            }
        }
        catch (const std::exception &e)
        {     
        //    std::cout << "TEST" <<std::endl;                                                           // specify the exception type
           throw; // re-throw the exception with a modified message
        }
    }
    else
    {
        _directiveValue._type = STRING;
        std::copy(directiveValue.begin(), directiveValue.begin() + 1024, _directiveValue._stringValue);
    }
}

directiveValueUnion	DirectiveValue::getDirectiveValue() const {
    return(_directiveValue);   
}

std::ostream &operator<<(std::ostream &o, DirectiveValue const &i)
{
    directiveValueUnion val;
    val = i.getDirectiveValue();
    if (val._type == INT) {
        o << val._intValue;   
    }
    else{
        o << val._stringValue;   
    }
	return o;
}