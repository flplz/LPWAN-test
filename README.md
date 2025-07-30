# Experimentos LPWAN - Sigfox, LoRaWAN e NB-IoT

Este projeto contém simulações completas de tecnologias LPWAN (Low Power Wide Area Network) usando o NS-3, incluindo Sigfox, LoRaWAN e NB-IoT.

## 📋 Pré-requisitos

### 1. Sistema Operacional
- Linux (recomendado Ubuntu 20.04+)
- WSL2 no Windows
- macOS (com algumas limitações)

### 2. Dependências do Sistema
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install build-essential git python3 python3-pip cmake

# CentOS/RHEL
sudo yum groupinstall "Development Tools"
sudo yum install git python3 python3-pip cmake
```

### 3. NS-3
```bash
# Clone o NS-3
git clone https://gitlab.com/nsnam/ns-3-dev.git
cd ns-3-dev

# Configure e compile
./ns3 configure --enable-examples --enable-tests
./ns3 build

# Instale módulos adicionais necessários
# Nota: Alguns módulos podem não estar disponíveis na versão padrão
```

### 4. Dependências Python
```bash
pip3 install pandas numpy matplotlib seaborn
```

## 🚀 Execução Rápida

### Opção 1: Execução Automatizada (Recomendada)
```bash
# Dar permissão de execução
chmod +x run_experiments.sh

# Executar todos os experimentos
./run_experiments.sh
```

### Opção 2: Execução Manual

#### 1. Sigfox
```bash
cd sigfox/
g++ -o sigfox_sim sigfox_simulacao_explicada.cc -I/usr/include/ns3 -lns3-core -lns3-network -lns3-mobility -lns3-sigfox -lns3-energy
./sigfox_sim --distance=1000 --nDevices=10
```

#### 2. LoRaWAN
```bash
cd lorawan/
g++ -o lorawan_sim lorawan-test.cc -I/usr/include/ns3 -lns3-core -lns3-network -lns3-mobility -lns3-lora -lns3-energy
./lorawan_sim --distance=1000 --deviceCount=10
```

#### 3. NB-IoT
```bash
cd nbiot/
g++ -o nbiot_sim nb-iot-sim.cc -I/usr/include/ns3 -lns3-core -lns3-network -lns3-mobility -lns3-lte -lns3-energy
./nbiot_sim --distance=1000 --nUe=10
```

## 📊 Métricas Coletadas

Todos os experimentos geram as seguintes métricas:

| Métrica | Descrição | Unidade |
|---------|-----------|---------|
| Execução | Número da execução | - |
| Distância (m) | Distância entre dispositivos e gateway | metros |
| Dispositivos | Número de dispositivos | - |
| Pacotes Enviados | Total de pacotes transmitidos | - |
| Pacotes Recebidos | Total de pacotes recebidos | - |
| Pacotes Duplicados | Pacotes duplicados (se aplicável) | - |
| PDR (%) | Packet Delivery Ratio | % |
| PLR (%) | Packet Loss Ratio | % |
| Retransmissão (%) | Taxa de retransmissão | % |
| Vazão (bps) | Throughput | bits/segundo |
| Utilização do Canal (%) | Utilização do canal | % |
| Atraso Médio (s) | Delay médio | segundos |
| Banda Utilizada (kHz) | Largura de banda | kHz |
| Consumo Energia (J) | Consumo de energia | Joules |

## 🔧 Scripts Disponíveis

### 1. `run_experiments.sh`
Script principal que executa todos os experimentos automaticamente.

**Configurações:**
- Tecnologias: Sigfox, LoRaWAN, NB-IoT
- Distâncias: 1000, 2000, 3000, 4000, 5000 metros
- Dispositivos: 10, 50, 100
- Execuções por cenário: 5

### 2. `converter_resultados.py`
Converte saídas dos experimentos para CSV padronizado.

**Uso:**
```bash
# Converter arquivo específico
python3 converter_resultados.py sigfox resultados_sigfox.txt

# Converter todos os arquivos
python3 converter_resultados.py --all
```

### 3. `analise_resultados.py`
Gera gráficos e relatórios estatísticos.

**Uso:**
```bash
python3 analise_resultados.py
```

## 📁 Estrutura do Projeto

```
LPWAN-test/
├── sigfox/
│   ├── sigfox_simulacao_explicada.cc
│   └── sigfox_explicacao.md
├── lorawan/
│   └── lorawan-test.cc
├── nbiot/
│   ├── nb-iot-sim.cc
│   └── nb-iot_explicacao.md
├── converter_resultados.py
├── analise_resultados.py
├── run_experiments.sh
├── GUIA_COMPLETO_EXPERIMENTOS_LPWAN.md
└── README.md
```

## 📈 Resultados Esperados

Após a execução, você terá:

### Arquivos de Dados
- `resultados_sigfox_*.csv`
- `resultados_lorawan_*.csv`
- `resultados_nbiot_*.csv`
- `tabela_comparativa.csv`

### Gráficos
- `comparacao_tecnologias.png` - Comparação geral
- `analise_sigfox.png` - Análise detalhada Sigfox
- `analise_lorawan.png` - Análise detalhada LoRaWAN
- `analise_nbiot.png` - Análise detalhada NB-IoT

### Relatórios
- `relatorio_experimentos.txt` - Relatório estatístico completo

## ⚠️ Problemas Comuns e Soluções

### 1. Erro de Compilação
```bash
# Verificar se o NS-3 está instalado
which ns3

# Verificar bibliotecas
ldconfig -p | grep ns3
```

### 2. Módulos NS-3 Não Encontrados
Alguns módulos (como Sigfox) podem não estar disponíveis na versão padrão do NS-3. Neste caso:
- Use apenas LoRaWAN e NB-IoT
- Ou procure por implementações alternativas

### 3. Erro de Permissão
```bash
chmod +x run_experiments.sh
```

### 4. Dependências Python
```bash
pip3 install --upgrade pandas numpy matplotlib seaborn
```

## 🔬 Reproduzibilidade

Para garantir a reprodutibilidade dos resultados:

1. **Seeds Fixos**: Todos os experimentos usam seeds fixos
2. **Parâmetros Padronizados**: Mesmos parâmetros para todas as tecnologias
3. **Ambiente Controlado**: Scripts automatizados garantem consistência

## 📚 Referências

- [NS-3 Documentation](https://www.nsnam.org/documentation/)
- [LoRaWAN Specification](https://lora-alliance.org/)
- [Sigfox Documentation](https://www.sigfox.com/)
- [NB-IoT Specification](https://www.3gpp.org/)

## 🤝 Contribuição

Para contribuir com o projeto:

1. Fork o repositório
2. Crie uma branch para sua feature
3. Commit suas mudanças
4. Push para a branch
5. Abra um Pull Request

## 📄 Licença

Este projeto está sob a licença MIT. Veja o arquivo LICENSE para mais detalhes.

## 📞 Suporte

Para dúvidas ou problemas:

1. Verifique a seção de problemas comuns
2. Consulte o guia completo: `GUIA_COMPLETO_EXPERIMENTOS_LPWAN.md`
3. Abra uma issue no repositório

---

**Nota**: Este projeto foi desenvolvido para fins acadêmicos e de pesquisa. Os resultados podem variar dependendo do ambiente de execução. 