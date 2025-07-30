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
#include "ns3/energy-module.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <set>
#include <unordered_map>

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

// Variáveis de energia
double totalEnergyConsumption = 0.0;
double initialEnergy = 3600.0; // Joules

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
    totalEnergyConsumption = 0.0;
}

// Funções de callback
void PacketSent(Ptr<LoraNetDevice> device, Ptr<Packet> packet) {
    uint32_t uid = packet->GetUid();
    packetsSent++;
    sentTimes[uid] = Simulator::Now();
    totalTransmitTime += 1.0;
    
    // Consumo de energia por transmissão (simplificado)
    totalEnergyConsumption += 0.1; // 0.1 J por transmissão
    
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

    double distance = 1000.0;  // Distância padrão
    uint32_t deviceCount = 10;  // Número padrão de dispositivos
    uint32_t payloadSize = 50;  // Payload padrão
    uint32_t runSeed = 12345;  // Seed padrão
    uint32_t numExecutions = 1;  // Uma execução por padrão

    CommandLine cmd;
    cmd.AddValue("distance", "Distância entre dispositivos e gateway (m)", distance);
    cmd.AddValue("deviceCount", "Número de dispositivos", deviceCount);
    cmd.AddValue("payloadSize", "Tamanho do payload (bytes)", payloadSize);
    cmd.AddValue("runSeed", "Seed para reprodutibilidade", runSeed);
    cmd.AddValue("numExecutions", "Número de execuções", numExecutions);
    cmd.Parse(argc, argv);

    RngSeedManager::SetSeed(runSeed);

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
        allocator->Add(Vector(0, 0, 0)); // Gateway
        allocator->Add(Vector(distance, 0, 0)); // Dispositivos
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
        gateways.Create(1); // Apenas um gateway
        mobility.Install(gateways);

        phyHelper.SetDeviceType(LoraPhyHelper::GW);
        macHelper.SetDeviceType(LorawanMacHelper::GW);
        NetDeviceContainer gatewayNetDevices = helper.Install(phyHelper, macHelper, gateways);

        // Conectar callbacks para dispositivos
        for (uint32_t i = 0; i < endDeviceNetDevices.GetN(); ++i) {
            Ptr<LoraNetDevice> dev = DynamicCast<LoraNetDevice>(endDeviceNetDevices.Get(i));
            dev->GetPhy()->TraceConnectWithoutContext("SendPacket", MakeCallback(&PacketSent));
            dev->SetReceiveCallback(MakeCallback(&PacketReceived));
        }

        // Conectar callbacks para gateways
        for (uint32_t j = 0; j < gatewayNetDevices.GetN(); ++j) {
            Ptr<LoraNetDevice> gatewayDev = DynamicCast<LoraNetDevice>(gatewayNetDevices.Get(j));
            gatewayDev->SetReceiveCallback(MakeCallback(&PacketReceived));
        }

        // Enviar pacotes
        for (uint32_t k = 0; k < endDevices.GetN(); ++k) {
            Ptr<LoraNetDevice> dev = DynamicCast<LoraNetDevice>(endDeviceNetDevices.Get(k));
            Ptr<Packet> packet = Create<Packet>(payloadSize);
            Simulator::Schedule(Seconds(1.0 + k), &PacketSent, dev, packet);
        }

        Simulator::Stop(Seconds(15));
        Simulator::Run();

        // Cálculo das métricas finais
        double pdr = (packetsSent > 0) ? (double(packetsReceived) / packetsSent) * 100 : 0.0;
        double plr = (packetsSent > 0) ? (double(packetsSent - packetsReceived) / packetsSent) * 100 : 0.0;
        double throughput = totalBytesReceived * 8 / 15.0; // bps
        double retransmissionRate = (packetsReceived > 0) ? (double(duplicatedPackets) / packetsReceived) * 100 : 0.0;
        double channelUtilization = (packetsSent > 0) ? 
            (totalTransmitTime / 15.0) * 100 : 0.0; // Duração fixa de 15s
        double averageDelay = (packetsReceived > 0) ? (totalDelay.GetSeconds() / packetsReceived) : 0.0;
        double bandwidthUsed = 125.0; // LoRaWAN usa 125 kHz
        double energyConsumption = totalEnergyConsumption;

        // Saída padronizada
        std::cout << "=== RESULTADOS LORAWAN ===" << std::endl;
        std::cout << "Execução: " << (run + 1) << std::endl;
        std::cout << "Distância (m): " << distance << std::endl;
        std::cout << "Dispositivos: " << deviceCount << std::endl;
        std::cout << "Pacotes Enviados: " << packetsSent << std::endl;
        std::cout << "Pacotes Recebidos: " << packetsReceived << std::endl;
        std::cout << "Pacotes Duplicados: " << duplicatedPackets << std::endl;
        std::cout << "PDR (%): " << pdr << std::endl;
        std::cout << "PLR (%): " << plr << std::endl;
        std::cout << "Retransmissão (%): " << retransmissionRate << std::endl;
        std::cout << "Vazão (bps): " << throughput << std::endl;
        std::cout << "Utilização do Canal (%): " << channelUtilization << std::endl;
        std::cout << "Atraso Médio (s): " << averageDelay << std::endl;
        std::cout << "Banda Utilizada (kHz): " << bandwidthUsed << std::endl;
        std::cout << "Consumo Energia (J): " << energyConsumption << std::endl;

        // Gerar arquivo CSV
        std::ofstream csvFile("resultados_lorawan.csv");
        csvFile << "Execução,Distância (m),Dispositivos,Pacotes Enviados,Pacotes Recebidos,Pacotes Duplicados,PDR (%),PLR (%),Retransmissão (%),Vazão (bps),Utilização do Canal (%),Atraso Médio (s),Banda Utilizada (kHz),Consumo Energia (J)\n";
        csvFile << (run + 1) << "," << distance << "," << deviceCount << "," << packetsSent << "," << packetsReceived << "," << duplicatedPackets << "," 
                << pdr << "," << plr << "," << retransmissionRate << "," << throughput << "," << channelUtilization << "," 
                << averageDelay << "," << bandwidthUsed << "," << energyConsumption << "\n";
        csvFile.close();

        Simulator::Destroy();
    }

    return 0;
}
