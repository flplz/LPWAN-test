#!/bin/bash

# Script para executar experimentos LPWAN com versões simplificadas
# Usa apenas módulos básicos do NS-3

echo "=== EXECUÇÃO DE EXPERIMENTOS LPWAN (VERSÃO SIMPLIFICADA) ==="
echo ""

# Configurações dos experimentos
TECNOLOGIAS=("sigfox" "lorawan" "nbiot")
DISTANCIAS=(1000 2000 3000 4000 5000)
DISPOSITIVOS=(10 50 100)
EXECUCOES=3

# Criar diretório para resultados
mkdir -p resultados_simplificado
cd resultados_simplificado

echo "Configurações:"
echo "- Tecnologias: ${TECNOLOGIAS[*]}"
echo "- Distâncias: ${DISTANCIAS[*]} metros"
echo "- Dispositivos: ${DISPOSITIVOS[*]}"
echo "- Execuções por cenário: $EXECUCOES"
echo ""

# Verificar se NS-3 está instalado
if [ ! -d "$HOME/ns3_test/ns-3-dev" ]; then
    echo "❌ NS-3 não encontrado!"
    echo "Execute primeiro:"
    echo "chmod +x instalar_ns3.sh"
    echo "./instalar_ns3.sh"
    exit 1
fi

NS3_PATH="$HOME/ns3_test/ns-3-dev"

# Função para executar experimento Sigfox simplificado
executar_sigfox_simplificado() {
    local distancia=$1
    local dispositivos=$2
    local execucao=$3
    
    echo "  Executando Sigfox (Simplificado) - Distância: ${distancia}m, Dispositivos: $dispositivos, Execução: $execucao"
    
    # Compilar se necessário
    if [ ! -f "../sigfox/sigfox_simplificado" ]; then
        echo "    Compilando Sigfox simplificado..."
        cd ../sigfox
        g++ -o sigfox_simplificado sigfox_simplificado.cc -I$NS3_PATH/build -I$NS3_PATH/src -L$NS3_PATH/build/lib -lns3-core -lns3-network -lns3-mobility -lns3-energy -lns3-applications -lns3-wifi -lns3-propagation 2>/dev/null
        cd ../resultados_simplificado
    fi
    
    # Executar simulação
    ../sigfox/sigfox_simplificado --distance=$distancia --nDevices=$dispositivos > "resultados_sigfox_${distancia}m_${dispositivos}dev_${execucao}.txt" 2>&1
    
    # Verificar se gerou CSV
    if [ -f "../sigfox/resultados_sigfox.csv" ]; then
        cp ../sigfox/resultados_sigfox.csv "resultados_sigfox_${distancia}m_${dispositivos}dev_${execucao}.csv"
    fi
}

# Função para executar experimento LoRaWAN simplificado
executar_lorawan_simplificado() {
    local distancia=$1
    local dispositivos=$2
    local execucao=$3
    
    echo "  Executando LoRaWAN (Simplificado) - Distância: ${distancia}m, Dispositivos: $dispositivos, Execução: $execucao"
    
    # Usar versão simplificada baseada em WiFi
    cd ../lorawan
    
    # Criar versão simplificada se não existir
    if [ ! -f "lorawan_simplificado.cc" ]; then
        echo "    Criando versão simplificada do LoRaWAN..."
        cat > lorawan_simplificado.cc << 'EOF'
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

NS_LOG_COMPONENT_DEFINE("LoRaWANSimplificado");

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
    totalEnergyConsumption += 0.1;
}

void RxTrace(Ptr<const Packet> packet, const Address &addr) {
    packetsReceived++;
    totalDelay += Simulator::Now().GetSeconds() - 1.0;
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
    loss->SetPathLossExponent(4.0);
    loss->SetReference(1, 10);

    Ptr<PropagationDelayModel> delay = CreateObject<ConstantSpeedPropagationDelayModel>();
    Ptr<YansWifiChannel> channel = CreateObject<YansWifiChannel>();
    channel->SetPropagationLossModel(loss);
    channel->SetPropagationDelayModel(delay);

    YansWifiPhyHelper phy;
    phy.SetChannel(channel);
    phy.Set("TxPowerStart", DoubleValue(14.0));
    phy.Set("TxPowerEnd", DoubleValue(14.0));

    WifiMacHelper mac;
    mac.SetType("ns3::AdhocWifiMac");

    WifiHelper wifi;
    wifi.SetStandard(WIFI_PHY_STANDARD_80211b);
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                "DataMode", StringValue("DsssRate1Mbps"),
                                "ControlMode", StringValue("DsssRate1Mbps"));

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
    serverApps.Start(Seconds(0.0));
    serverApps.Stop(Seconds(15.0));

    UdpEchoClientHelper echoClient(gateway.Get(0)->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal(), port);
    echoClient.SetAttribute("MaxPackets", UintegerValue(5));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(50));

    for (uint32_t i = 0; i < endDevices.GetN(); ++i) {
        clientApps.Add(echoClient.Install(endDevices.Get(i)));
    }

    clientApps.Start(Seconds(1.0));
    clientApps.Stop(Seconds(15.0));

    Config::ConnectWithoutContext("/NodeList/*/ApplicationList/*/$ns3::UdpEchoClient/Tx", MakeCallback(&TxTrace));
    Config::ConnectWithoutContext("/NodeList/*/ApplicationList/*/$ns3::UdpEchoServer/Rx", MakeCallback(&RxTrace));

    Simulator::Stop(Seconds(15.0));
    Simulator::Run();

    double pdr = packetsSent > 0 ? ((double)packetsReceived / packetsSent) * 100 : 0;
    double plr = packetsSent > 0 ? ((double)(packetsSent - packetsReceived) / packetsSent) * 100 : 0;
    double retransmissionRate = 0.0;
    double throughput = totalThroughput / 15.0;
    double avgDelay = packetsReceived > 0 ? totalDelay / packetsReceived : 0;
    double bandwidthUsed = 125.0;
    double channelUtilization = (packetsSent * 50 * 8) / (15.0 * bandwidthUsed * 1000) * 100;

    std::cout << "=== RESULTADOS LORAWAN (SIMPLIFICADO) ===" << std::endl;
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

    std::ofstream csvFile("resultados_lorawan.csv");
    csvFile << "Execução,Distância (m),Dispositivos,Pacotes Enviados,Pacotes Recebidos,Pacotes Duplicados,PDR (%),PLR (%),Retransmissão (%),Vazão (bps),Utilização do Canal (%),Atraso Médio (s),Banda Utilizada (kHz),Consumo Energia (J)\n";
    csvFile << "1," << distance << "," << nDevices << "," << packetsSent << "," << packetsReceived << "," << packetsDuplicated << "," 
            << pdr << "," << plr << "," << retransmissionRate << "," << throughput << "," << channelUtilization << "," 
            << avgDelay << "," << bandwidthUsed << "," << totalEnergyConsumption << "\n";
    csvFile.close();

    Simulator::Destroy();
    return 0;
}
EOF
    fi
    
    # Compilar se necessário
    if [ ! -f "lorawan_simplificado" ]; then
        echo "    Compilando LoRaWAN simplificado..."
        g++ -o lorawan_simplificado lorawan_simplificado.cc -I$NS3_PATH/build -I$NS3_PATH/src -L$NS3_PATH/build/lib -lns3-core -lns3-network -lns3-mobility -lns3-energy -lns3-applications -lns3-wifi -lns3-propagation 2>/dev/null
    fi
    
    # Executar simulação
    ./lorawan_simplificado --distance=$distancia --nDevices=$dispositivos > "../resultados_simplificado/resultados_lorawan_${distancia}m_${dispositivos}dev_${execucao}.txt" 2>&1
    
    # Verificar se gerou CSV
    if [ -f "resultados_lorawan.csv" ]; then
        cp resultados_lorawan.csv "../resultados_simplificado/resultados_lorawan_${distancia}m_${dispositivos}dev_${execucao}.csv"
    fi
    
    cd ../resultados_simplificado
}

# Função para executar experimento NB-IoT simplificado
executar_nbiot_simplificado() {
    local distancia=$1
    local dispositivos=$2
    local execucao=$3
    
    echo "  Executando NB-IoT (Simplificado) - Distância: ${distancia}m, Dispositivos: $dispositivos, Execução: $execucao"
    
    # Usar versão simplificada baseada em WiFi
    cd ../nbiot
    
    # Criar versão simplificada se não existir
    if [ ! -f "nbiot_simplificado.cc" ]; then
        echo "    Criando versão simplificada do NB-IoT..."
        cat > nbiot_simplificado.cc << 'EOF'
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
EOF
    fi
    
    # Compilar se necessário
    if [ ! -f "nbiot_simplificado" ]; then
        echo "    Compilando NB-IoT simplificado..."
        g++ -o nbiot_simplificado nbiot_simplificado.cc -I$NS3_PATH/build -I$NS3_PATH/src -L$NS3_PATH/build/lib -lns3-core -lns3-network -lns3-mobility -lns3-energy -lns3-applications -lns3-wifi -lns3-propagation 2>/dev/null
    fi
    
    # Executar simulação
    ./nbiot_simplificado --distance=$distancia --nDevices=$dispositivos > "../resultados_simplificado/resultados_nbiot_${distancia}m_${dispositivos}dev_${execucao}.txt" 2>&1
    
    # Verificar se gerou CSV
    if [ -f "resultados_nbiot.csv" ]; then
        cp resultados_nbiot.csv "../resultados_simplificado/resultados_nbiot_${distancia}m_${dispositivos}dev_${execucao}.csv"
    fi
    
    cd ../resultados_simplificado
}

# Executar todos os experimentos
for tech in "${TECNOLOGIAS[@]}"; do
    echo "Executando experimentos para $tech..."
    
    for dist in "${DISTANCIAS[@]}"; do
        for dev in "${DISPOSITIVOS[@]}"; do
            echo "  Cenário: ${dist}m, $dev dispositivos"
            
            for exec in $(seq 1 $EXECUCOES); do
                case $tech in
                    "sigfox")
                        executar_sigfox_simplificado $dist $dev $exec
                        ;;
                    "lorawan")
                        executar_lorawan_simplificado $dist $dev $exec
                        ;;
                    "nbiot")
                        executar_nbiot_simplificado $dist $dev $exec
                        ;;
                esac
                
                sleep 1
            done
        done
    done
    echo ""
done

echo "=== CONVERSÃO PARA CSV ==="
echo "Convertendo resultados para CSV..."

cd ..

# Executar script de conversão
if [ -f "converter_resultados.py" ]; then
    python3 converter_resultados.py --all
else
    echo "Script de conversão não encontrado!"
fi

echo "=== ANÁLISE DOS RESULTADOS ==="
echo "Gerando gráficos e relatórios..."

# Executar script de análise
if [ -f "analise_resultados.py" ]; then
    python3 analise_resultados.py
else
    echo "Script de análise não encontrado!"
fi

echo ""
echo "=== EXPERIMENTOS CONCLUÍDOS (VERSÃO SIMPLIFICADA) ==="
echo "Resultados salvos em: resultados_simplificado/"
echo "CSVs gerados em: resultados_simplificado/"
echo "Gráficos e relatórios gerados no diretório atual"
echo ""
echo "Esta versão usa apenas módulos básicos do NS-3 e simula"
echo "o comportamento das tecnologias LPWAN usando WiFi como base."
echo ""
echo "Para reproduzir os experimentos, execute:"
echo "  chmod +x run_experiments_simplificado.sh"
echo "  ./run_experiments_simplificado.sh" 