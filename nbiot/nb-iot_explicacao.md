# Simulação NB-IoT com NS-3

Este script simula uma rede NB-IoT utilizando o módulo LTE do NS-3. Ele foi adaptado para capturar métricas de rede como:

- Pacotes Enviados
- Pacotes Recebidos
- PDR (Packet Delivery Ratio)
- PLR (Packet Loss Ratio)
- Vazão (Throughput)
- Atraso Médio
- Banda Utilizada

## 💡 Lógica de Funcionamento

1. **Criação dos nós:**
   - Um eNB central (base station)
   - Vários UEs (usuários), posicionados a `X` metros de distância

2. **Mobilidade:**
   - eNB fixo em (0,0)
   - UEs fixos na mesma distância `X`

3. **Configuração de rede:**
   - Ponto-a-ponto entre PGW e Remote Host
   - Endereçamento IP, rotas e gateway padrão

4. **Instalação de dispositivos:**
   - LTE para eNB e UEs
   - Internet nas UEs

5. **Aplicações:**
   - Cliente UDP Echo (UE) → envia N pacotes
   - Servidor UDP Echo (Remote Host) → recebe

6. **Cálculo de métricas:**
   - Pacotes são contados nos callbacks de Tx/Rx
   - Atraso médio = soma dos tempos de chegada − tempo inicial
   - Vazão = bits recebidos / tempo total
   - PDR = (Rx / Tx) * 100
   - PLR = ((Tx - Rx) / Tx) * 100

7. **Resultados:**
   - Impresso no terminal (pode ser exportado para CSV por script externo)

## ⚙️ Parâmetros que podem ser ajustados:

- `nUe` — número de dispositivos
- `distance` — distância entre eNB e UEs
- `packetsPerUe` — quantidade de pacotes por UE
- `simTime` — duração da simulação

---

### 👇 Exemplo de uso:

```bash
./waf --run "nb-iot-sim --nUe=500 --distance=3000 --packetsPerUe=12"
