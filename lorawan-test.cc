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
#include <algorithm>
#include <ctime>

using namespace ns3;
using namespace lorawan;

NS_LOG_COMPONENT_DEFINE("SimpleLorawanNetworkExample");

// Variáveis para armazenar métricas
uint32_t packetsSent = 0;
uint32_t packetsReceived = 0;
Time totalDelay;
std::set<uint32_t> receivedUids;  // Para armazenar UIDs de pacotes já recebidos

// Função para enviar o pacote e capturar o envio
void PacketSent(Ptr<LoraNetDevice> device, Ptr<Packet> packet) {
    NS_LOG_INFO("Enviando pacote de " << packet->GetSize() << " bytes. UID: " << packet->GetUid());
    packetsSent++;
    device->Send(packet);
}

// Função de callback para recepção de pacotes
bool PacketReceived(Ptr<NetDevice> dev, Ptr<const Packet> packet, uint16_t, const Address&) {
    uint32_t uid = packet->GetUid();
    NS_LOG_INFO("Pacote recebido com UID: " << uid << ", tamanho: " << packet->GetSize());

    // Verificar se o UID já foi recebido para evitar duplicação
    if (receivedUids.find(uid) == receivedUids.end()) {
        receivedUids.insert(uid);
        packetsReceived++;
        Time delay = Simulator::Now();  // Calcular atraso
        totalDelay += delay;
        NS_LOG_INFO("Pacote processado. UID: " << uid << ", tamanho: " << packet->GetSize());
        return true;
    } else {
        NS_LOG_WARN("Pacote duplicado ignorado. UID: " << uid << ", tamanho: " << packet->GetSize());
        return false;  // Pacote duplicado
    }
}

int main(int argc, char* argv[]) {
    // Configuração de logging
    LogComponentEnable("SimpleLorawanNetworkExample", LOG_LEVEL_INFO);

    /************************
     *  Criação do canal  *
     ************************/
    NS_LOG_INFO("Creating the channel...");
    Ptr<LogDistancePropagationLossModel> loss = CreateObject<LogDistancePropagationLossModel>();
    loss->SetPathLossExponent(3.76);
    loss->SetReference(1, 7.7);

    Ptr<PropagationDelayModel> delay = CreateObject<ConstantSpeedPropagationDelayModel>();
    Ptr<LoraChannel> channel = CreateObject<LoraChannel>(loss, delay);

    // Adicionando sub-banda AU915 (915 MHz a 928 MHz)
    LogicalLoraChannelHelper logicalChannelHelper;
    logicalChannelHelper.AddSubBand(915000000, 928000000, 0.01, 30);

    /************************
     *  Configuração dos helpers  *
     ************************/
    NS_LOG_INFO("Setting up helpers...");
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> allocator = CreateObject<ListPositionAllocator>();
    allocator->Add(Vector(1000, 0, 0));
    allocator->Add(Vector(0, 0, 0));
    mobility.SetPositionAllocator(allocator);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");

    // Configuração dos helpers LoRa
    LoraPhyHelper phyHelper = LoraPhyHelper();
    phyHelper.SetChannel(channel);

    LorawanMacHelper macHelper = LorawanMacHelper();
    LoraHelper helper = LoraHelper();

    /************************
     *  Criação dos dispositivos finais  *
     ************************/
    NS_LOG_INFO("Creating the end devices...");
    NodeContainer endDevices;
    endDevices.Create(10);
    mobility.Install(endDevices);

    // Instalar dispositivos finais
    phyHelper.SetDeviceType(LoraPhyHelper::ED);
    macHelper.SetDeviceType(LorawanMacHelper::ED_A);
    NetDeviceContainer endDeviceNetDevices = helper.Install(phyHelper, macHelper, endDevices);

    /*********************
     *  Criação dos gateways  *
     *********************/
    NS_LOG_INFO("Creating the gateway...");
    NodeContainer gateways;
    gateways.Create(1);
    mobility.Install(gateways);

    // Instalar gateways
    phyHelper.SetDeviceType(LoraPhyHelper::GW);
    macHelper.SetDeviceType(LorawanMacHelper::GW);
    NetDeviceContainer gatewayNetDevices = helper.Install(phyHelper, macHelper, gateways);

    /*******************************
     *  Conectar callbacks de métricas  *
     *******************************/
    // Para dispositivos finais
    for (uint32_t i = 0; i < endDeviceNetDevices.GetN(); ++i) {
        Ptr<LoraNetDevice> dev = DynamicCast<LoraNetDevice>(endDeviceNetDevices.Get(i));
        NS_ASSERT(dev != nullptr);
        dev->GetPhy()->TraceConnectWithoutContext("SendPacket", MakeCallback(&PacketSent));
        dev->SetReceiveCallback(MakeCallback(&PacketReceived));
    }

    // Para gateways
    Ptr<LoraNetDevice> gatewayDev = DynamicCast<LoraNetDevice>(gatewayNetDevices.Get(0));
    NS_ASSERT(gatewayDev != nullptr);
    gatewayDev->SetReceiveCallback(MakeCallback(&PacketReceived));

    /*********************************************
     *  Instalar aplicações nos dispositivos  *
     *********************************************/
    OneShotSenderHelper oneShotSenderHelper;
    oneShotSenderHelper.SetSendTime(Seconds(5));  // Ajustar intervalo para 5 segundos
    oneShotSenderHelper.Install(endDevices);

    // Enviar pacote explicitamente
    Ptr<LoraNetDevice> dev = DynamicCast<LoraNetDevice>(endDeviceNetDevices.Get(0));
    Ptr<Packet> packet = Create<Packet>(50); // Pacote de 50 bytes
    Simulator::Schedule(Seconds(1.0), &PacketSent, dev, packet);  // Agendar o envio do pacote

    /****************
     *  Simulação  *
     ****************/
    Simulator::Stop(Seconds(15));  // Tempo de simulação ajustado
    Simulator::Run();

    // Exibir métricas no final
    NS_LOG_INFO("********** Métricas da Simulação **********");
    NS_LOG_INFO("Pacotes enviados: " << packetsSent);
    NS_LOG_INFO("Pacotes recebidos: " << packetsReceived);
    if (packetsSent > 0) {
        NS_LOG_INFO("Pacotes perdidos: " << (packetsSent - packetsReceived));
    } else {
        NS_LOG_INFO("Nenhum pacote enviado.");
    }
    if (packetsReceived > 0) {
        NS_LOG_INFO("Atraso médio: " << totalDelay.GetSeconds() / packetsReceived << " segundos");
    }

    Simulator::Destroy();
    return 0;
}
