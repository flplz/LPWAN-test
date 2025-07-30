#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/energy-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/propagation-module.h"
#include <fstream>
#include <iostream>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("NbIotSimplificado");

uint32_t packetsSent = 0;
uint32_t packetsReceived = 0;
uint32_t packetsDuplicated = 0;
double totalDelay = 0.0;
double totalThroughput = 0.0;
double totalEnergyConsumption = 0.0;
double distance = 1000.0;
uint32_t nDevices = 10;

void TxTrace(Ptr<const Packet> packet) {
    packetsSent++;
    totalEnergyConsumption += 0.05;
}

void RxTrace(Ptr<const Packet> packet, const Address &addr) {
    packetsReceived++;
    totalDelay += Simulator::Now().GetSeconds() - 2.0;
    totalThroughput += packet->GetSize() * 8;
}

int main(int argc, char *argv[]) {
    CommandLine cmd;
    cmd.AddValue("distance", "Distância (m)", distance);
    cmd.AddValue("nDevices", "Número de dispositivos", nDevices);
    cmd.Parse(argc, argv);

    RngSeedManager::SetSeed(12345);
    RngSeedManager::SetRun(1);

    NodeContainer endDevices, gateway;
    endDevices.Create(nDevices);
    gateway.Create(1);

    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));
    for (uint32_t i = 0; i < nDevices; ++i) {
        positionAlloc->Add(Vector(distance, 0.0 + i * 10, 0.0));
    }
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(gateway);
    mobility.Install(endDevices);

    Ptr<LogDistancePropagationLossModel> loss = CreateObject<LogDistancePropagationLossModel>();
    loss->SetPathLossExponent(3.5);
    loss->SetReference(1, 8);

    Ptr<PropagationDelayModel> delay = CreateObject<ConstantSpeedPropagationDelayModel>();
    Ptr<YansWifiChannel> channel = CreateObject<YansWifiChannel>();
    channel->SetPropagationLossModel(loss);
    channel->SetPropagationDelayModel(delay);

    YansWifiPhyHelper phy;
    phy.SetChannel(channel);
    phy.Set("TxPowerStart", DoubleValue(23.0));
    phy.Set("TxPowerEnd", DoubleValue(23.0));

    WifiMacHelper mac;
    mac.SetType("ns3::AdhocWifiMac");

    WifiHelper wifi;
    wifi.SetStandard(WIFI_PHY_STANDARD_80211b);
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                "DataMode", StringValue("DsssRate11Mbps"),
                                "ControlMode", StringValue("DsssRate11Mbps"));

    NetDeviceContainer endDeviceDevices = wifi.Install(phy, mac, endDevices);
    NetDeviceContainer gatewayDevices = wifi.Install(phy, mac, gateway);

    BasicEnergySourceHelper energySource;
    energySource.Set("BasicEnergySourceInitialEnergyJ", DoubleValue(3600.0));
    EnergySourceContainer sources = energySource.Install(endDevices);

    WifiRadioEnergyModelHelper radioEnergyHelper;
    DeviceEnergyModelContainer deviceModels = radioEnergyHelper.Install(endDeviceDevices, sources);

    uint16_t port = 8000;
    ApplicationContainer serverApps, clientApps;

    UdpEchoServerHelper echoServer(port);
    serverApps = echoServer.Install(gateway.Get(0));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    UdpEchoClientHelper echoClient(gateway.Get(0)->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal(), port);
    echoClient.SetAttribute("MaxPackets", UintegerValue(10));
    echoClient.SetAttribute("Interval", TimeValue(MilliSeconds(500)));
    echoClient.SetAttribute("PacketSize", UintegerValue(50));

    for (uint32_t i = 0; i < endDevices.GetN(); ++i) {
        clientApps.Add(echoClient.Install(endDevices.Get(i)));
    }

    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));

    Config::ConnectWithoutContext("/NodeList/*/ApplicationList/*/$ns3::UdpEchoClient/Tx", MakeCallback(&TxTrace));
    Config::ConnectWithoutContext("/NodeList/*/ApplicationList/*/$ns3::UdpEchoServer/Rx", MakeCallback(&RxTrace));

    Simulator::Stop(Seconds(10.0));
    Simulator::Run();

    double pdr = packetsSent > 0 ? ((double)packetsReceived / packetsSent) * 100 : 0;
    double plr = packetsSent > 0 ? ((double)(packetsSent - packetsReceived) / packetsSent) * 100 : 0;
    double retransmissionRate = 0.0;
    double throughput = totalThroughput / 10.0;
    double avgDelay = packetsReceived > 0 ? totalDelay / packetsReceived : 0;
    double bandwidthUsed = 180.0;
    double channelUtilization = (packetsSent * 50 * 8) / (10.0 * bandwidthUsed * 1000) * 100;

    std::cout << "=== RESULTADOS NB-IOT (SIMPLIFICADO) ===" << std::endl;
    std::cout << "Execução: 1" << std::endl;
    std::cout << "Distância (m): " << distance << std::endl;
    std::cout << "Dispositivos: " << nDevices << std::endl;
    std::cout << "Pacotes Enviados: " << packetsSent << std::endl;
    std::cout << "Pacotes Recebidos: " << packetsReceived << std::endl;
    std::cout << "Pacotes Duplicados: " << packetsDuplicated << std::endl;
    std::cout << "PDR (%): " << pdr << std::endl;
    std::cout << "PLR (%): " << plr << std::endl;
    std::cout << "Retransmissão (%): " << retransmissionRate << std::endl;
    std::cout << "Vazão (bps): " << throughput << std::endl;
    std::cout << "Utilização do Canal (%): " << channelUtilization << std::endl;
    std::cout << "Atraso Médio (s): " << avgDelay << std::endl;
    std::cout << "Banda Utilizada (kHz): " << bandwidthUsed << std::endl;
    std::cout << "Consumo Energia (J): " << totalEnergyConsumption << std::endl;

    std::ofstream csvFile("resultados_nbiot.csv");
    csvFile << "Execução,Distância (m),Dispositivos,Pacotes Enviados,Pacotes Recebidos,Pacotes Duplicados,PDR (%),PLR (%),Retransmissão (%),Vazão (bps),Utilização do Canal (%),Atraso Médio (s),Banda Utilizada (kHz),Consumo Energia (J)\n";
    csvFile << "1," << distance << "," << nDevices << "," << packetsSent << "," << packetsReceived << "," << packetsDuplicated << "," 
            << pdr << "," << plr << "," << retransmissionRate << "," << throughput << "," << channelUtilization << "," 
            << avgDelay << "," << bandwidthUsed << "," << totalEnergyConsumption << "\n";
    csvFile.close();

    Simulator::Destroy();
    return 0;
}
