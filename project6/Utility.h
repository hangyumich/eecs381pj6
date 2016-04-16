#ifndef UTILITY_H
#define UTILITY_H

#include <exception>

// This Exception class is used for general error
class Error : public std::exception {
public:
	Error(const char* msg_) : msg(msg_) {}
    const char* what() const noexcept override
        {return msg;}
private:
	const char* msg;
};

/* A special exception. During group command execution, this kind of exception
 will not abort the command. Inherit from Error so that we don't have to change
 the previous error handling code. */
class Not_have_ability : public Error {
public:
    Not_have_ability(const char* msg_): Error(msg_){}
};


#endif
