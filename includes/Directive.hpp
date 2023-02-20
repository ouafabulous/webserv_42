#ifndef DIRECTIVE_HPP
# define DIRECTIVE_HPP

#include "Type.hpp"
#include "Config.hpp"


typedef enum s_type{
	INT,
	STRING
}			t_type;

union directiveValueUnion {
		int		_intValue;
	    char	_stringValue[DIRECTIVE_NAME_LENGTH];
};

class Directive {
	public:
		Directive(std::string directiveName);
		~Directive(){};
		std::vector<directiveValueUnion> const &	getDirectiveValues() const;
		void										addDirectiveValue(std::string DirectiveValue);
		t_type const &								getType() const;
		std::string	const	&						getDirectiveName() const;

	private:
		t_type								_type;
		std::vector<directiveValueUnion>	_directiveValues;
		std::string							_directiveName;
};

std::ostream &			operator<<( std::ostream & o, Directive const & i );

# endif