
#ifndef __CRYPT_HPP
#define __CRYPT_HPP

#include <string>

namespace wx
{
namespace crypt
{

std::string encrypt_data(const std::string& Data, const std::string& Key);

std::string decrypt_data(const std::string& Data, const std::string& Key);

std::string encrypt_string(const std::string& Data, const std::string& Key);

std::string decrypt_string(const std::string& Data, const std::string& Key);

}  // namespace crypt
}  // namespace wx

#endif  // __CRYPT_HPP