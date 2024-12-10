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

#include <set>
#include <unordered_map>

using namespace ns3;
using namespace lorawan;

NS_LOG_COMPONENT_DEFINE("SimpleLorawanNetworkExample");

// Variáveis para armazenar métricas
uint32_t packetsSent = 0;
uint32_t packetsReceived = 0;
uint32_t duplicatedPackets = 0;
double totalBytesReceived = 0;
double totalTransmitTime = 0.0;
Time totalDelay;
Time simulationStartTime;

std::set<uint32_t> receivedUids;      // UIDs de pacotes únicos recebidos
std::unordered_map<uint32_t, Time> sentTimes;  // Tempos de envio

// Função para enviar pacotes
void PacketSent(Ptr<LoraNetDevice> device, Ptr<Packet> packet) {
    uint32_t uid = packet->GetUid();
    NS_LOG_INFO("Pacote enviado com UID: " << uid << " pelo dispositivo " << device->GetNode()->GetId());
    packetsSent++;
    sentTimes[uid] = Simulator::Now();  // Armazena tempo de envio
    totalTransmitTime += 1.0;  // Incrementa o tempo de transmissão
    device->Send(packet);
}

// Função para recepção de pacotes
bool PacketReceived(Ptr<NetDevice> dev, Ptr<const Packet> packet, uint16_t, const Address&) {
    uint32_t uid = packet->GetUid();
    uint32_t packetSize = packet->GetSize();
    NS_LOG_INFO("Pacote recebido com UID: " << uid << ", tamanho: " << packetSize);

    // Contabilizar pacotes duplicados
    if (receivedUids.find(uid) != receivedUids.end()) {
        duplicatedPackets++;
        NS_LOG_WARN("Pacote duplicado ignorado. UID: " << uid);
        return false;
    }

    // Processar pacotes únicos
    receivedUids.insert(uid);
    packetsReceived++;
    totalBytesReceived += packetSize;

    // Calcula o atraso
    if (sentTimes.find(uid) != sentTimes.end()) {
        Time delay = Simulator::Now() - sentTimes[uid];
        totalDelay += delay;
        sentTimes.erase(uid);  // Remove tempo de envio para liberar memória
    }
    NS_LOG_INFO("Pacote processado. UID: " << uid << ", tamanho: " << packetSize);
    return true;
}

int main(int argc, char* argv[]) {
    // Configuração de logging
    LogComponentEnable("SimpleLorawanNetworkExample", LOG_LEVEL_INFO);

    /*************************
     *  Criação do Canal    *
     *************************/
    NS_LOG_INFO("Criando o canal...");
    Ptr<LogDistancePropagationLossModel> loss = CreateObject<LogDistancePropagationLossModel>();
    loss->SetPathLossExponent(3.76);
    loss->SetReference(1, 7.7);

    Ptr<PropagationDelayModel> delay = CreateObject<ConstantSpeedPropagationDelayModel>();
    Ptr<LoraChannel> channel = CreateObject<LoraChannel>(loss, delay);

    LogicalLoraChannelHelper logicalChannelHelper;
    logicalChannelHelper.AddSubBand(915000000, 928000000, 0.01, 30);

    /*************************
     *  Configuração dos Helpers   *
     *************************/
    NS_LOG_INFO("Configurando os helpers...");
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> allocator = CreateObject<ListPositionAllocator>();
    allocator->Add(Vector(0, 0, 0));      // Gateway 1
    allocator->Add(Vector(500, 500, 0));  // Gateway 2
    mobility.SetPositionAllocator(allocator);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");

    LoraPhyHelper phyHelper = LoraPhyHelper();
    phyHelper.SetChannel(channel);

    LorawanMacHelper macHelper = LorawanMacHelper();
    LoraHelper helper = LoraHelper();

    /*************************
     *  Criação dos Dispositivos Finais   *
     *************************/
    NS_LOG_INFO("Criando dispositivos finais...");
    NodeContainer endDevices;
    endDevices.Create(10);
    mobility.Install(endDevices);

    phyHelper.SetDeviceType(LoraPhyHelper::ED);
    macHelper.SetDeviceType(LorawanMacHelper::ED_A);
    NetDeviceContainer endDeviceNetDevices = helper.Install(phyHelper, macHelper, endDevices);

    /*************************
     *  Criação dos Gateways   *
     *************************/
    NS_LOG_INFO("Criando gateways...");
    NodeContainer gateways;
    gateways.Create(2);
    mobility.Install(gateways);

    phyHelper.SetDeviceType(LoraPhyHelper::GW);
    macHelper.SetDeviceType(LorawanMacHelper::GW);
    NetDeviceContainer gatewayNetDevices = helper.Install(phyHelper, macHelper, gateways);

    /*************************
     *  Conectar Callbacks   *
     *************************/
    for (uint32_t i = 0; i < endDeviceNetDevices.GetN(); ++i) {
        Ptr<LoraNetDevice> dev = DynamicCast<LoraNetDevice>(endDeviceNetDevices.Get(i));
        dev->GetPhy()->TraceConnectWithoutContext("SendPacket", MakeCallback(&PacketSent));
        dev->SetReceiveCallback(MakeCallback(&PacketReceived));
    }

    for (uint32_t j = 0; j < gatewayNetDevices.GetN(); ++j) {
        Ptr<LoraNetDevice> gatewayDev = DynamicCast<LoraNetDevice>(gatewayNetDevices.Get(j));
        gatewayDev->SetReceiveCallback(MakeCallback(&PacketReceived));
    }

    /*************************
     *  Instalar Aplicações  *
     *************************/
    OneShotSenderHelper oneShotSenderHelper;
    oneShotSenderHelper.SetSendTime(Seconds(10));

    for (uint32_t k = 0; k < endDevices.GetN(); ++k) {
        Ptr<LoraNetDevice> dev = DynamicCast<LoraNetDevice>(endDeviceNetDevices.Get(k));
        Ptr<Packet> packet = Create<Packet>(50); 
        Simulator::Schedule(Seconds(1.0 + k), &PacketSent, dev, packet);
    }

    /*******************
     *  Simulação  *
     *******************/
    simulationStartTime = Simulator::Now();
    Simulator::Stop(Seconds(15));
    Simulator::Run();

    // Calcular métricas
    double pdr = (packetsSent > 0) ? (double(packetsReceived) / packetsSent) * 100 : 0.0;
    double plr = (packetsSent > 0) ? (double(packetsSent - packetsReceived) / packetsSent) * 100 : 0.0;
    double retransmissionRate = (packetsReceived > 0) ? (double(duplicatedPackets) / packetsReceived) * 100 : 0.0;
    double throughput = (Simulator::Now() - simulationStartTime).GetSeconds() > 0 ?
        (totalBytesReceived * 8 / (Simulator::Now() - simulationStartTime).GetSeconds()) : 0.0;
    double channelUtilization = (packetsSent > 0) ?
        (totalTransmitTime / (Simulator::Now() - simulationStartTime).GetSeconds()) * 100 : 0.0;

    // Exibir métricas
    NS_LOG_INFO("********** Métricas da Simulação **********");
    NS_LOG_INFO("Pacotes enviados: " << packetsSent);
    NS_LOG_INFO("Pacotes recebidos: " << packetsReceived);
    NS_LOG_INFO("Pacotes duplicados: " << duplicatedPackets);
    NS_LOG_INFO("PDR (Taxa de Entrega): " << pdr << "%");
    NS_LOG_INFO("PLR (Taxa de Perda): " << plr << "%");
    NS_LOG_INFO("Retransmissão: " << retransmissionRate << "%");
    NS_LOG_INFO("Vazão: " << throughput << " bps");
    NS_LOG_INFO("Utilização do Canal: " << channelUtilization << "%");
    NS_LOG_INFO("Atraso Médio: " << (packetsReceived > 0 ? (totalDelay.GetSeconds() / packetsReceived) : 0.0) << " s");

    Simulator::Destroy();
    return 0;
}
