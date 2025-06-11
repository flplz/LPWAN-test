
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

using namespace ns3;
using namespace sigfox;

// Variáveis globais
int nDevices = 1;
int nGateways = 1;
const int TotalTime = 62; // Tempo total da simulação em segundos

double battery = 100 * 60 * 60; // Energia total da bateria (360000 J)
double EnergyConsumptionMeasurment = 0;
double EnergyConsumptionNode = 0;
double TotalRemainingEnergy = 0;

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
}

void LostPacketAtGateway (Ptr<const Packet> packet, uint32_t id) {
  SigfoxTag tag;
  packet->PeekPacketTag (tag);
  SetPacketOutcome (tag.GetSenderId(), tag.GetPacketNumber(), tag.GetRepetitionNumber(), false);
}

int main (int argc, char *argv[]) {
  CommandLine cmd;
  cmd.AddValue ("nDevices", "Número de dispositivos Sigfox", nDevices);
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
  allocator->Add (Vector (0, 0, 0));
  allocator->Add (Vector (0, 0, 0));
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

  gateways.Get (0)->GetDevice (0)->GetObject<SigfoxNetDevice> ()->GetPhy ()->TraceConnectWithoutContext("ReceivedPacket", MakeCallback(&ReceivedPacketAtGateway));
  gateways.Get (0)->GetDevice (0)->GetObject<SigfoxNetDevice> ()->GetPhy ()->TraceConnectWithoutContext("LostPacketBecauseInterference", MakeCallback(&LostPacketAtGateway));

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

  auto [successes, failures] = CountSuccessesAndFailures();
  std::cout << successes << " " << failures << std::endl;
  return 0;
}
