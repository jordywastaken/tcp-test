
namespace General
{
    void TestConnectivity(int s)
    {
        Socket::SendInt(s, 1337);
    }

    void Notify(int s)
    {
        auto* packet = Socket::ReceiveType<NotifyPacket>(s);

        //auto texture = GetTexture(packet->icon);
        //SendNotification(texture, packet->text);
        delete packet;
    }
}