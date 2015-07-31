//#include "precompile.h"
#include "crypt.hpp"
#include <stdlib.h>
#include <time.h>
#include <Windows.h>
#include "tea.h"
#include "modp_b64.h"

namespace wx
{
namespace crypt
{

#define AlignedToSize(_Amount, _Size)                      \
    (ULONG)((((ULONG_PTR)(_Amount)) + ((_Size)-1)) & ~(ULONG_PTR) ((_Size) - 1))

std::string encrypt_data(const std::string& Data, const std::string& Key)
{
    std::string Ciphetrext("");
    if (!Key.empty())
    {
        srand(static_cast<unsigned int>(time(nullptr)));
        int RandKey = rand() % 256;

        char Buffer[16] = {0};
        memset(Buffer, 0, sizeof(Buffer));
        sprintf_s(Buffer, 16, "%02X", RandKey);
        Ciphetrext += Buffer;

        std::string::const_iterator KeyIter = Key.begin();
        std::string::const_iterator DataIter = Data.begin();
        for (; DataIter != Data.end(); DataIter++)
        {
            unsigned char c = (unsigned char)(*DataIter);
            int stream = (c + RandKey) % 255;
            stream ^= (unsigned char)(*KeyIter);

            memset(Buffer, 0, sizeof(Buffer));
            sprintf_s(Buffer, 16, "%02X", stream);
            Ciphetrext += Buffer;

            KeyIter++;
            if (KeyIter == Key.end())
                KeyIter = Key.begin();

            RandKey = stream;
        }
    }

    return std::string(Ciphetrext);
}

std::string decrypt_data(const std::string& Data, const std::string& Key)
{
    std::string OriData("");

    if (Data.size() > 2)
    {
        const char *DataBuffer = Data.c_str();
        const char *EndDataBuffer = Data.c_str() + Data.size();
        char Buffer[16] = {0};
        memset(Buffer, 0, sizeof(Buffer));
        memcpy(Buffer, DataBuffer, 2);
        char *EndPtr = nullptr;
        int Offset = (int)strtol(Buffer, &EndPtr, 16);
        DataBuffer += 2;

        std::string::const_iterator KeyIter = Key.begin();
        for (; DataBuffer < EndDataBuffer; DataBuffer += 2)
        {
            memset(Buffer, 0, sizeof(Buffer));
            memcpy(Buffer, DataBuffer, 2);
            int Stream = (int)strtol(Buffer, &EndPtr, 16);
            int OriStream = Stream ^ (unsigned char)(*KeyIter);
            if (OriStream <= Offset)
                OriStream = 255 + OriStream - Offset;
            else
                OriStream -= Offset;

            Offset = Stream;
            OriData.push_back((char)OriStream);

            KeyIter++;
            if (KeyIter == Key.end())
                KeyIter = Key.begin();
        }
    }

    return std::string(OriData);
}

std::string decrypt_string(const std::string& Data, const std::string& Key)
{
    std::string result("");
    std::string crypt_data = Data;
    std::string decrypt_data = modp_b64_decode(crypt_data);
    if (decrypt_data.size() > 4)
    {
        int org_size = *(int *)(decrypt_data.c_str() + decrypt_data.size() - 4);
        std::string temp_data(decrypt_data.c_str(), decrypt_data.size() - 4);
        size_t size = AlignedToSize(temp_data.size(), 8);
        if (size == temp_data.size())
        {
            size /= 8;
            for (size_t i = 0; i < size; i ++)
            {
                tea_decrypt((unsigned long *)(&temp_data[0] + i * 8), (unsigned long *)Key.c_str());
            }
            result.append(temp_data.c_str(), org_size);
        }
    }

    return std::string(result);
}

std::string encrypt_string(const std::string& Data, const std::string& Key)
{
    int size = AlignedToSize(Data.size() + 1, 8); // 最后结尾的0也要算进去
    size = size / 8;
    int diff_size = (size * 8) - Data.size();
    std::string crypt_data = Data;
    std::string padding;
    padding.resize(diff_size);
    crypt_data.append(padding.c_str(), padding.size());
    for (int i = 0; i < size; i++)
    {
        tea_encrypt((unsigned long *)(&crypt_data[0] + i * 8), (unsigned long *)Key.c_str());
    }
    size = Data.size();
    crypt_data.append((char *)&size, 4);
    return modp_b64_encode(crypt_data);
}

}  // namespace crypt
}  // namespace wx