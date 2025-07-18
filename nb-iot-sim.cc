/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Simulação NB-IoT com cálculo de métricas
 * Base: exemplo LteEpc
 * Autor: Felipe F. Pacheco (adaptado com ChatGPT)
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/lte-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/ipv4-global-routing-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("NbIotSim");

// Variáveis globais de métricas
uint32_t totalTx = 0;
uint32_t totalRx = 0;
uint32_t totalRetx = 0;
double totalDelay = 0.0;
Time startTime;

void TxTrace(Ptr<const Packet> p)
{
  totalTx++;
}

void RxTrace(Ptr<const Packet> p, const Address &addr)
{
  totalRx++;
  totalDelay += Simulator::Now().GetSeconds() - startTime.GetSeconds();
}

int main(int argc, char *argv[])
{
  uint32_t nUe = 10;
  double simTime = 10.0;
  double distance = 3000.0;
  uint32_t packetsPerUe = 10;

  CommandLine cmd;
  cmd.AddValue("nUe", "Número de UEs", nUe);
  cmd.AddValue("simTime", "Tempo de simulação (s)", simTime);
  cmd.AddValue("distance", "Distância entre eNB e UEs (m)", distance);
  cmd.AddValue("packetsPerUe", "Qtd de pacotes por UE", packetsPerUe);
  cmd.Parse(argc, argv);

  // Helpers principais
  Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();
  Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper>();
  lteHelper->SetEpcHelper(epcHelper);

  // Internet para o Remote Host
  NodeContainer remoteHostContainer;
  remoteHostContainer.Create(1);
  Ptr<Node> remoteHost = remoteHostContainer.Get(0);
  InternetStackHelper internet;
  internet.Install(remoteHostContainer);

  Ptr<Node> pgw = epcHelper->GetPgwNode();
  PointToPointHelper p2ph;
  p2ph.SetDeviceAttribute("DataRate", StringValue("100Gbps"));
  p2ph.SetChannelAttribute("Delay", TimeValue(Seconds(0.01)));
  NetDeviceContainer internetDevices = p2ph.Install(pgw, remoteHost);
  Ipv4AddressHelper ipv4h;
  ipv4h.SetBase("1.0.0.0", "255.0.0.0");
  ipv4h.Assign(internetDevices);

  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  Ptr<Ipv4StaticRouting> remoteRouting = ipv4RoutingHelper.GetStaticRouting(remoteHost->GetObject<Ipv4>());
  remoteRouting->AddNetworkRouteTo(Ipv4Address("7.0.0.0"), Ipv4Mask("255.0.0.0"), 1);

  // Criar eNB e UEs
  NodeContainer ueNodes;
  NodeContainer enbNodes;
  enbNodes.Create(1);
  ueNodes.Create(nUe);

  // Mobilidade
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
  positionAlloc->Add(Vector(0.0, 0.0, 0.0)); // eNB fixo
  for (uint32_t i = 0; i < nUe; ++i)
  {
    positionAlloc->Add(Vector(distance, 0.0 + i, 0.0)); // UEs na mesma distância
  }
  mobility.SetPositionAllocator(positionAlloc);
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(enbNodes);
  mobility.Install(ueNodes);

  // Instala dispositivos LTE
  NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice(enbNodes);
  NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice(ueNodes);

  // Instala internet e IP
  internet.Install(ueNodes);
  Ipv4InterfaceContainer ueIpIface = epcHelper->AssignUeIpv4Address(NetDeviceContainer(ueLteDevs));

  for (uint32_t i = 0; i < ueNodes.GetN(); ++i)
  {
    Ptr<Node> ueNode = ueNodes.Get(i);
    Ptr<Ipv4StaticRouting> ueRouting = ipv4RoutingHelper.GetStaticRouting(ueNode->GetObject<Ipv4>());
    ueRouting->SetDefaultRoute(epcHelper->GetUeDefaultGatewayAddress(), 1);
  }

  // Attach
  for (uint32_t i = 0; i < ueNodes.GetN(); ++i)
  {
    lteHelper->Attach(ueLteDevs.Get(i), enbLteDevs.Get(0));
  }

  // Aplicações (UDP Echo como exemplo de envio)
  uint16_t port = 8000;
  ApplicationContainer serverApps, clientApps;
  for (uint32_t i = 0; i < ueNodes.GetN(); ++i)
  {
    UdpEchoServerHelper echoServer(port);
    serverApps.Add(echoServer.Install(remoteHost));

    UdpEchoClientHelper echoClient(remoteHost->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal(), port);
    echoClient.SetAttribute("MaxPackets", UintegerValue(packetsPerUe));
    echoClient.SetAttribute("Interval", TimeValue(MilliSeconds(500)));
    echoClient.SetAttribute("PacketSize", UintegerValue(50));
    clientApps.Add(echoClient.Install(ueNodes.Get(i)));
  }

  serverApps.Start(Seconds(1.0));
  clientApps.Start(Seconds(2.0));
  startTime = Seconds(2.0);

  // Traços de métricas
  Config::ConnectWithoutContext("/NodeList/*/ApplicationList/*/$ns3::UdpEchoClient/Tx", MakeCallback(&TxTrace));
  Config::ConnectWithoutContext("/NodeList/*/ApplicationList/*/$ns3::UdpEchoServer/Rx", MakeCallback(&RxTrace));

  Simulator::Stop(Seconds(simTime));
  Simulator::Run();

  // Cálculo das métricas
  double pdr = totalTx > 0 ? ((double)totalRx / totalTx) * 100 : 0;
  double plr = totalTx > 0 ? ((double)(totalTx - totalRx) / totalTx) * 100 : 0;
  double atrasoMedio = totalRx > 0 ? totalDelay / totalRx : 0;
  double vazao = totalRx * 50 * 8 / simTime; // bps
  double bandaUtilizada = 125;

  std::cout << "Distancia: " << distance << "m\n";
  std::cout << "Dispositivos: " << nUe << "\n";
  std::cout << "Pacotes Enviados: " << totalTx << "\n";
  std::cout << "Pacotes Recebidos: " << totalRx << "\n";
  std::cout << "PDR(%): " << pdr << "\n";
  std::cout << "PLR(%): " << plr << "\n";
  std::cout << "Atraso Medio(s): " << atrasoMedio << "\n";
  std::cout << "Vazão(bps): " << vazao << "\n";
  std::cout << "Banda Utilizada (kHz): " << bandaUtilizada << "\n";

  Simulator::Destroy();
  return 0;
}
