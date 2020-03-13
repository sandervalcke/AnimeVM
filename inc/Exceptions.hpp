
#ifndef __EXCEPTIONS_HPP__
#define __EXCEPTIONS_HPP__

#include <string>
#include <sstream>


class Exception
{
public:
  Exception(const char* what) : what_(what) {}
  Exception(std::string what) : what_(what) { /* empty body */ }
  Exception(QString what) : what_(what.toStdString()) { /* empty body */ }

  std::string what() const { return what_; }

private:
  std::string what_;
};

class IOError : public Exception
{
public:
  IOError(std::string what) : Exception(what) { /* empty body */ }
};


template<typename T>
std::string str(T x)
{
  std::ostringstream o;
  if (!(o << x))
    throw Exception("error in str");
  return o.str();
}

#endif // __EXCEPTIONS_HPP__
