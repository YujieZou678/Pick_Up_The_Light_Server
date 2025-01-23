#include "netpacketgenerator.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include <string>
using std::string;

NetPacketGenerator::NetPacketGenerator()
{
}

NetPacketGenerator *NetPacketGenerator::getInstance()
{
    static NetPacketGenerator instance;  //局部静态变量初始化线程安全 C++11
    return &instance;
}

NetPacket NetPacketGenerator::register_P(bool success)
{
    NetPacket p;
    json jsonMsg;

    if (success) {
        jsonMsg["success"] = "true";
    } else {
        jsonMsg["success"] = "false";
    }

    /* 数据包 */
    string strMsg = jsonMsg.dump();
    strcpy(p.dataPacket.data, strMsg.data());
    /* 包头 */
    p.packetHeader.purpose = Purpose::Register;
    p.packetHeader.data_size = strMsg.size();

    return p;
}
