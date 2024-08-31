#include "Utils.hpp"

String Utils::GetSubStr(const String& p_rawMsg, int p_startId, int p_endId, bool p_verbose)
{
    if ((p_startId < 0))
    {
        return "";
    }
    int end = p_endId == -1 ? p_rawMsg.size() - 1 : p_endId;

    String subStr = p_rawMsg.substr(p_startId, p_endId - p_startId);
#ifdef PC_BUILD
    if (p_verbose)
        std::cout << subStr << std::endl;
#endif
    return subStr;
}
