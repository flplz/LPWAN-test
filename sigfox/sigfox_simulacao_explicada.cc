
/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Exemplo de Simulação Sigfox no NS-3 com análise de energia e entrega de pacotes
 * Baseado em código da University of Padova, adaptado para simulações de longo alcance.
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/sigfox-module.h"
#include "ns3/energy-module.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace ns3;
using namespace sigfox;

// Variáveis globais
int nDevices = 1;
int nGateways = 1;
double distance = 1000.0; // Distância em metros
const int TotalTime = 62; // Tempo total da simulação em segundos

double battery = 100 * 60 * 60; // Energia total da bateria (360000 J)
double EnergyConsumptionMeasurment = 0;
double EnergyConsumptionNode = 0;
double TotalRemainingEnergy = 0;

// Métricas de rede
uint32_t packetsSent = 0;
uint32_t packetsReceived = 0;
uint32_t packetsDuplicated = 0;
double totalDelay = 0.0;
double totalThroughput = 0.0;
double channelUtilization = 0.0;
double bandwidthUsed = 100.0; // Sigfox usa 100 kHz

// Funções de callback e rastreamento de energia
void Print () {
  if (TotalRemainingEnergy >= 0)
    TotalRemainingEnergy = battery - EnergyConsumptionNode - EnergyConsumptionMeasurment;
  if (TotalRemainingEnergy <= 0)
    TotalRemainingEnergy = 0;
  std::ofstream out ("BatteryLevel.txt", std::ios::app);
  out << Simulator::Now ().GetSeconds () << " , " << TotalRemainingEnergy << std::endl;
  out.close ();
  Simulator::Schedule (Seconds (60.0), &Print);
}

void Measure () {
  EnergyConsumptionMeasurment += 32.2;
  Simulator::Schedule (Seconds (60.0), &Measure);
}

void TotalEnergy (double oldValue, double totalEnergy) {
  EnergyConsumptionNode = totalEnergy;
}

// Funções para análise de pacotes
std::vector<std::vector<std::vector<bool>>> packetOutcomes;
void SetPacketOutcome (uint32_t senderId, uint8_t appPacket, uint8_t repetitionNumber, bool outcome) {
  std::vector<std::vector<bool>> &senderOutcomes = packetOutcomes.at (senderId);
  if (appPacket >= senderOutcomes.size ()) {
    if (Simulator::Now () > Seconds (10) && Simulator::Now () < Seconds (TotalTime - 10)) {
      senderOutcomes.push_back ({outcome});
    }
  } else {
    senderOutcomes.at (appPacket).push_back (outcome);
  }
}

std::pair<int, int> CountSuccessesAndFailures () {
  int failures = 0, successes = 0;
  for (auto &senderOutcomes : packetOutcomes) {
    for (auto &outcomes : senderOutcomes) {
      if (std::none_of (outcomes.begin (), outcomes.end (), [] (bool v) {return v;})) failures++;
      else successes++;
    }
  }
  return {successes, failures};
}

void ReceivedPacketAtGateway (Ptr<const Packet> packet, uint32_t id) {
  SigfoxTag tag;
  packet->PeekPacketTag (tag);
  SetPacketOutcome (tag.GetSenderId(), tag.GetPacketNumber(), tag.GetRepetitionNumber(), true);
  packetsReceived++;
  totalDelay += Simulator::Now().GetSeconds() - 10.0; // Tempo desde início
  totalThroughput += packet->GetSize() * 8; // bits
}

void LostPacketAtGateway (Ptr<const Packet> packet, uint32_t id) {
  SigfoxTag tag;
  packet->PeekPacketTag (tag);
  SetPacketOutcome (tag.GetSenderId(), tag.GetPacketNumber(), tag.GetRepetitionNumber(), false);
}

void PacketSentCallback (Ptr<const Packet> packet) {
  packetsSent++;
}

int main (int argc, char *argv[]) {
  CommandLine cmd;
  cmd.AddValue ("nDevices", "Número de dispositivos Sigfox", nDevices);
  cmd.AddValue ("distance", "Distância entre dispositivos e gateway (m)", distance);
  cmd.Parse (argc, argv);

  // Canal
  Ptr<LogDistancePropagationLossModel> loss = CreateObject<LogDistancePropagationLossModel> ();
  loss->SetPathLossExponent (3.76);
  loss->SetReference (1, 7.7);
  Ptr<PropagationDelayModel> delay = CreateObject<ConstantSpeedPropagationDelayModel> ();
  Ptr<SigfoxChannel> channel = CreateObject<SigfoxChannel> (loss, delay);

  // Mobilidade
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> allocator = CreateObject<ListPositionAllocator> ();
  allocator->Add (Vector (0, 0, 0)); // Gateway
  allocator->Add (Vector (distance, 0, 0)); // Dispositivo
  mobility.SetPositionAllocator (allocator);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

  // Instalação dos dispositivos e gateways
  NodeContainer endDevices, gateways;
  endDevices.Create (nDevices);
  gateways.Create (nGateways);
  mobility.Install (endDevices);
  mobility.Install (gateways);

  SigfoxHelper helper;
  SigfoxPhyHelper phyHelper;
  SigfoxMacHelper macHelper;
  phyHelper.SetChannel (channel);
  phyHelper.SetDeviceType (SigfoxPhyHelper::EP);
  macHelper.SetDeviceType (SigfoxMacHelper::EP);
  NetDeviceContainer endDevicesNetDevices = helper.Install (phyHelper, macHelper, endDevices);
  packetOutcomes.resize(nDevices);

  phyHelper.SetDeviceType (SigfoxPhyHelper::GW);
  macHelper.SetDeviceType (SigfoxMacHelper::GW);
  helper.Install (phyHelper, macHelper, gateways);

  // Conectar callbacks
  gateways.Get (0)->GetDevice (0)->GetObject<SigfoxNetDevice> ()->GetPhy ()->TraceConnectWithoutContext("ReceivedPacket", MakeCallback(&ReceivedPacketAtGateway));
  gateways.Get (0)->GetDevice (0)->GetObject<SigfoxNetDevice> ()->GetPhy ()->TraceConnectWithoutContext("LostPacketBecauseInterference", MakeCallback(&LostPacketAtGateway));
  endDevices.Get (0)->GetDevice (0)->GetObject<SigfoxNetDevice> ()->GetPhy ()->TraceConnectWithoutContext("SendPacket", MakeCallback(&PacketSentCallback));

  // Energia
  SdcEnergySourceHelper energyHelper;
  SigfoxRadioEnergyModelHelper radioHelper;
  energyHelper.Set ("SdcEnergySourceInitialEnergyJ", DoubleValue (3600));
  EnergySourceContainer sources = energyHelper.Install (endDevices);
  DeviceEnergyModelContainer devices = radioHelper.Install (endDevicesNetDevices, sources);

  Ptr<SdcEnergySource> energy = DynamicCast<SdcEnergySource> (sources.Get (0));
  energy->TraceConnectWithoutContext ("RemainingEnergy", MakeCallback (&TotalEnergy));

  // Aplicação
  PeriodicSenderHelper appHelper;
  appHelper.SetPeriod (Seconds (60));
  appHelper.SetPacketSize (12);
  ApplicationContainer apps = appHelper.Install (endDevices);
  apps.Start (Seconds (0));
  apps.Stop (Seconds (TotalTime));

  // Simulação
  Simulator::Schedule (Seconds (60.0), &Print);
  Simulator::Schedule (Seconds (60.0), &Measure);
  Simulator::Stop (Seconds (TotalTime));
  Simulator::Run ();
  Simulator::Destroy ();

  // Cálculo das métricas finais
  auto [successes, failures] = CountSuccessesAndFailures();
  
  double pdr = packetsSent > 0 ? ((double)packetsReceived / packetsSent) * 100 : 0;
  double plr = packetsSent > 0 ? ((double)(packetsSent - packetsReceived) / packetsSent) * 100 : 0;
  double retransmissionRate = 0.0; // Sigfox não tem retransmissão explícita
  double throughput = totalThroughput / TotalTime; // bps
  double avgDelay = packetsReceived > 0 ? totalDelay / packetsReceived : 0;
  double energyConsumption = battery - TotalRemainingEnergy;
  
  // Calcular utilização do canal (simplificado)
  channelUtilization = (packetsSent * 12 * 8) / (TotalTime * bandwidthUsed * 1000) * 100; // %

  // Saída padronizada
  std::cout << "=== RESULTADOS SIGFOX ===" << std::endl;
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
  std::cout << "Consumo Energia (J): " << energyConsumption << std::endl;

  // Gerar arquivo CSV
  std::ofstream csvFile("resultados_sigfox.csv");
  csvFile << "Execução,Distância (m),Dispositivos,Pacotes Enviados,Pacotes Recebidos,Pacotes Duplicados,PDR (%),PLR (%),Retransmissão (%),Vazão (bps),Utilização do Canal (%),Atraso Médio (s),Banda Utilizada (kHz),Consumo Energia (J)\n";
  csvFile << "1," << distance << "," << nDevices << "," << packetsSent << "," << packetsReceived << "," << packetsDuplicated << "," 
          << pdr << "," << plr << "," << retransmissionRate << "," << throughput << "," << channelUtilization << "," 
          << avgDelay << "," << bandwidthUsed << "," << energyConsumption << "\n";
  csvFile.close();

  return 0;
}
