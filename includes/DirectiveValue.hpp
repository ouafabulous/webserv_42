#ifndef DIRECTIVEVALUE
# define DIRECTIVEVALUE

#include "Type.hpp"


typedef enum s_type{
	INT,
	STRING
}			t_type;

union directiveValueUnion {
		t_type	_type;

		int		_intValue;
	    char	_stringValue[1023];
		std::string	*test;
		// std::string	stringValue;
};

class DirectiveValue {
	public:
		DirectiveValue(std::string directiveName, std::string directiveValue);
		~DirectiveValue(){};
		directiveValueUnion	getDirectiveValue() const;

	private:
		directiveValueUnion	_directiveValue;
};

std::ostream &			operator<<( std::ostream & o, DirectiveValue const & i );

# endif