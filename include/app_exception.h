#ifndef __APP_EXCEPTION_H___
#define __APP_EXCEPTION_H___

#include <exception>
#include <string>

class AppException : public std::exception
{
private:
    std::string msg;
public:
    AppException() throw() : msg("Unknown exception") {}
    
    AppException(const char* what) : msg(what) {}
    
    AppException& operator=(const AppException& other)
    {
        this->msg = other.msg;
        return *this;
    }

    virtual ~AppException() throw() {}

    virtual const char* what() const throw() 
    {
        return msg.c_str();
    }
};

#endif // __APP_EXCEPTION_H___
