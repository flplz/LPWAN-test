#include "ns3/command-line.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/end-device-lora-phy.h"
#include "ns3/end-device-lorawan-mac.h"
#include "ns3/gateway-lora-phy.h"
#include "ns3/gateway-lorawan-mac.h"
#include "ns3/log.h"
#include "ns3/lora-helper.h"
#include "ns3/mobility-helper.h"
#include "ns3/node-container.h"
#include "ns3/one-shot-sender-helper.h"
#include "ns3/position-allocator.h"
#include "ns3/simulator.h"
#include "ns3/logical-lora-channel-helper.h"
#include "ns3/rng-seed-manager.h"

using namespace ns3;
using namespace lorawan;

NS_LOG_COMPONENT_DEFINE("SimpleLorawanNetworkExample");

// Variáveis para armazenar métricas
uint32_t packetsSent;
uint32_t packetsReceived;
uint32_t duplicatedPackets;
double totalBytesReceived;
double totalTransmitTime;
Time totalDelay;
std::set<uint32_t> receivedUids;
std::unordered_map<uint32_t, Time> sentTimes;

// Função para reiniciar as métricas entre execuções
void ResetMetrics() {
    packetsSent = 0;
    packetsReceived = 0;
    duplicatedPackets = 0;
    totalBytesReceived = 0.0;
    totalTransmitTime = 0.0;
    totalDelay = Seconds(0);
    receivedUids.clear();
    sentTimes.clear();
}

// Funções de callback
void PacketSent(Ptr<LoraNetDevice> device, Ptr<Packet> packet) {
    uint32_t uid = packet->GetUid();
    packetsSent++;
    sentTimes[uid] = Simulator::Now();
    totalTransmitTime += 1.0;
    device->Send(packet);
}

bool PacketReceived(Ptr<NetDevice> dev, Ptr<const Packet> packet, uint16_t, const Address&) {
    uint32_t uid = packet->GetUid();
    uint32_t packetSize = packet->GetSize();

    if (receivedUids.find(uid) != receivedUids.end()) {
        duplicatedPackets++;
        return false;
    }

    receivedUids.insert(uid);
    packetsReceived++;
    totalBytesReceived += packetSize;

    if (sentTimes.find(uid) != sentTimes.end()) {
        Time delay = Simulator::Now() - sentTimes[uid];
        totalDelay += delay;
        sentTimes.erase(uid);
    }

    return true;
}

int main(int argc, char* argv[]) {
    LogComponentEnable("SimpleLorawanNetworkExample", LOG_LEVEL_INFO);

    double distance = 50000;  // Aumenta a distância
    uint32_t deviceCount = 100;  // Maior número de dispositivos
    uint32_t payloadSize = 50;  // Payload maior
    uint32_t runSeed = 12345;  // Novo seed para experimentos distintos
    uint32_t numExecutions = 10;  // Reduz número de execuções para teste

    RngSeedManager::SetSeed(runSeed + 12345);

    for (uint32_t run = 0; run < numExecutions; run++) {
        NS_LOG_INFO("Execução " << run + 1);

        ResetMetrics();
        RngSeedManager::SetRun(run + 1);  // Define um número único para cada execução

        // Configuração do canal e dispositivos
        Ptr<LogDistancePropagationLossModel> loss = CreateObject<LogDistancePropagationLossModel>();
        loss->SetPathLossExponent(4.00);
        loss->SetReference(1, 10);

        Ptr<PropagationDelayModel> delay = CreateObject<RandomPropagationDelayModel>();
        Ptr<LoraChannel> channel = CreateObject<LoraChannel>(loss, delay);

        MobilityHelper mobility;
        Ptr<ListPositionAllocator> allocator = CreateObject<ListPositionAllocator>();
        allocator->Add(Vector(0, 0, 0));
        allocator->Add(Vector(distance, distance, 0));
        mobility.SetPositionAllocator(allocator);
        mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");

        LogicalLoraChannelHelper logicalChannelHelper;
        logicalChannelHelper.AddSubBand(915000000, 928000000, 0.01, 30);

        LoraPhyHelper phyHelper;
        phyHelper.SetChannel(channel);

        LorawanMacHelper macHelper;
        LoraHelper helper;

        NodeContainer endDevices;
        endDevices.Create(deviceCount);
        mobility.Install(endDevices);

        phyHelper.SetDeviceType(LoraPhyHelper::ED);
        macHelper.SetDeviceType(LorawanMacHelper::ED_A);
        NetDeviceContainer endDeviceNetDevices = helper.Install(phyHelper, macHelper, endDevices);

        NodeContainer gateways;
        gateways.Create(2);
        mobility.Install(gateways);

        phyHelper.SetDeviceType(LoraPhyHelper::GW);
        macHelper.SetDeviceType(LorawanMacHelper::GW);
        NetDeviceContainer gatewayNetDevices = helper.Install(phyHelper, macHelper, gateways);

        for (uint32_t i = 0; i < endDeviceNetDevices.GetN(); ++i) {
            Ptr<LoraNetDevice> dev = DynamicCast<LoraNetDevice>(endDeviceNetDevices.Get(i));
            dev->GetPhy()->TraceConnectWithoutContext("SendPacket", MakeCallback(&PacketSent));
            dev->SetReceiveCallback(MakeCallback(&PacketReceived));
        }

        for (uint32_t j = 0; j < gatewayNetDevices.GetN(); ++j) {
            Ptr<LoraNetDevice> gatewayDev = DynamicCast<LoraNetDevice>(gatewayNetDevices.Get(j));
            gatewayDev->SetReceiveCallback(MakeCallback(&PacketReceived));
        }

        OneShotSenderHelper oneShotSenderHelper;
        oneShotSenderHelper.SetSendTime(Seconds(10));

        for (uint32_t k = 0; k < endDevices.GetN(); ++k) {
            Ptr<LoraNetDevice> dev = DynamicCast<LoraNetDevice>(endDeviceNetDevices.Get(k));
            Ptr<Packet> packet = Create<Packet>(payloadSize);
            Simulator::Schedule(Seconds(1.0 + k), &PacketSent, dev, packet);
        }

        Simulator::Stop(Seconds(15));
        Simulator::Run();

        // Exibir métricas detalhadas para cada execução
        double pdr = (packetsSent > 0) ? (double(packetsReceived) / packetsSent) * 100 : 0.0;
        double plr = (packetsSent > 0) ? (double(packetsSent - packetsReceived) / packetsSent) * 100 : 0.0;
        double throughput = totalBytesReceived * 8 / 15.0;
        double retransmissionRate = (packetsReceived > 0) ? (double(duplicatedPackets) / packetsReceived) * 100 : 0.0;
        double channelUtilization = (packetsSent > 0) ? 
            (totalTransmitTime / 15.0) * 100 : 0.0; // Duração fixa de 15s usada
        double averageDelay = (packetsReceived > 0) ? (totalDelay.GetSeconds() / packetsReceived) : 0.0;

        NS_LOG_INFO("********** Resultados da Execução " << run + 1 << " **********");
        NS_LOG_INFO("Distância: " << distance << " m, Dispositivos: " << deviceCount);
        NS_LOG_INFO("Pacotes enviados: " << packetsSent);
        NS_LOG_INFO("Pacotes recebidos: " << packetsReceived);
        NS_LOG_INFO("Pacotes duplicados: " << duplicatedPackets);
        NS_LOG_INFO("PDR (Taxa de Entrega): " << pdr << "%");
        NS_LOG_INFO("PLR (Taxa de Perda): " << plr << "%");
        NS_LOG_INFO("Retransmissão: " << retransmissionRate << "%");
        NS_LOG_INFO("Vazão: " << throughput << " bps");
        NS_LOG_INFO("Utilização do Canal: " << channelUtilization << "%");
        NS_LOG_INFO("Atraso Médio: " << averageDelay << " s");

        Simulator::Destroy();
    }

    return 0;
}
