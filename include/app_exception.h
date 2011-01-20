/*
 *   This file is part of arx-unpacker.
 *
 *   arx-unpacker is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   arx-unpacker is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with arx-unpacker.  If not, see <http://www.gnu.org/licenses/>
 */
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
