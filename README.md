# 📡 PROJETO LPWAN - ANÁLISE COMPARATIVA DE TECNOLOGIAS

## 🎯 RESUMO DO PROJETO

Este projeto realiza uma **análise comparativa abrangente** das principais tecnologias LPWAN (Low Power Wide Area Network): **Sigfox**, **LoRaWAN** e **NB-IoT**. 

### 📊 **Escala dos Experimentos:**
- **1.440 experimentos** realizados
- **3 tecnologias** analisadas
- **6 distâncias** (3km → 50km)
- **8 densidades** (10 → 50.000 dispositivos)
- **10 execuções** por configuração

### 🏆 **Principais Resultados:**
- **NB-IoT:** Melhor performance geral (PDR 93.5%, menor latência)
- **LoRaWAN:** Equilíbrio perfeito (performance + eficiência)
- **Sigfox:** Mais eficiente energeticamente (132J)

---

## 📁 ESTRUTURA DO PROJETO

```
LPWAN-test/
├── 📊 analise/                    # Scripts e relatórios de análise
│   ├── analise_simples.py         # Análise rápida
│   ├── analise_completa.py        # Análise completa + gráficos
│   ├── analise_completa_graficos.png
│   ├── analise_completa_radar.png
│   ├── graficos_lpwan.png
│   ├── GUIA_ANALISE_RESULTADOS.md
│   ├── RELATORIO_FINAL_ANALISE.md
│   └── RESUMO_ANALISE_FINAL.md
├── 📋 documentacao/               # Documentação técnica
│   ├── GUIA_COMPLETO_EXPERIMENTOS_LPWAN.md
│   ├── METODOLOGIA_SIMULACAO_LPWAN.md
│   ├── RELATORIO_ORIENTADOR.md
│   ├── RESUMO_FINAL.md
│   └── DIFERENCAS_SCRIPTS.md
├── 📈 resultados/                 # Dados dos experimentos
│   ├── resultados_extendidos_completo.csv
│   ├── resultados_extendidos_sigfox.csv
│   ├── resultados_extendidos_lorawan.csv
│   ├── resultados_extendidos_nbiot.csv
│   └── resultados_extendidos/     # Dados brutos
├── 🔧 scripts/                    # Scripts de automação
│   ├── gerar_experimentos_extendidos_simples.sh
│   ├── converter_extendidos_csv.py
│   ├── instalar_ns3.sh
│   └── verificar_instalacao.sh
├── 💻 codigos/                    # Códigos das simulações
├── 📖 README.md                   # Este arquivo
├── 📋 ESTRUTURA_PROJETO.md       # Estrutura detalhada
└── 🚫 .gitignore                  # Arquivos ignorados pelo Git
```

---

## 🚀 COMO USAR

### **1. 📊 ANÁLISE RÁPIDA DOS RESULTADOS**

Para uma análise básica dos dados:
```bash
cd analise
python3 analise_simples.py
```

### **2. 📈 ANÁLISE COMPLETA**

Para análise detalhada com gráficos e relatório:
```bash
cd analise
python3 analise_completa.py
```

### **3. 📖 LEITURA DOS RELATÓRIOS**

- **`analise/RELATORIO_FINAL_ANALISE.md`** - Relatório principal
- **`analise/GUIA_ANALISE_RESULTADOS.md`** - Guia de análise
- **`analise/RESUMO_ANALISE_FINAL.md`** - Resumo executivo

### **4. 🎨 VISUALIZAÇÕES**

Os gráficos gerados estão em `analise/`:
- `analise_completa_graficos.png` - Gráficos comparativos
- `analise_completa_radar.png` - Gráfico de radar
- `graficos_lpwan.png` - Versão alternativa

---

## 📊 PRINCIPAIS DESCOBERTAS

### **🏆 Ranking das Tecnologias:**

| Posição | Tecnologia | PDR | Vazão | Eficiência | Atraso |
|---------|------------|-----|-------|------------|--------|
| **🥇 1º** | **NB-IoT** | 93.5% | 9.8M bps | 265J | 5.25s |
| **🥈 2º** | **LoRaWAN** | 82.7% | 7.1M bps | 168J | 9.75s |
| **🥉 3º** | **Sigfox** | 82.5% | 7.3M bps | 132J | 15.67s |

### **💡 Insights Principais:**

#### **NB-IoT - Campeão Geral**
- ✅ Melhor PDR (93.5%)
- ✅ Maior vazão (9.8M bps)
- ✅ Menor atraso (5.25s)
- ❌ Maior consumo energético

#### **LoRaWAN - Equilibrista**
- ✅ Bom equilíbrio geral
- ✅ Performance intermediária
- ✅ Flexibilidade
- ⚠️ Trade-offs moderados

#### **Sigfox - Eficiente**
- ✅ Mais eficiente energeticamente
- ✅ Menor consumo (132J)
- ❌ Menor confiabilidade
- ❌ Maior latência

---

## 🎯 RECOMENDAÇÕES POR CENÁRIO

### **🏥 Aplicações Críticas (Saúde, Segurança)**
- **Tecnologia:** NB-IoT
- **Justificativa:** Maior confiabilidade (93.5% PDR)
- **Aplicação:** Monitoramento médico, sistemas de emergência

### **🏭 IoT Industrial**
- **Tecnologia:** LoRaWAN
- **Justificativa:** Equilíbrio performance/eficiência
- **Aplicação:** Monitoramento de equipamentos, controle industrial

### **🌱 IoT Ambiental (Longa Duração)**
- **Tecnologia:** Sigfox
- **Justificativa:** Maior eficiência energética (132J)
- **Aplicação:** Sensores ambientais, agricultura

### **🏙️ Smart Cities**
- **Recomendação:** Híbrida
- **Crítico:** NB-IoT (semáforos, emergências)
- **Geral:** LoRaWAN (sensores, iluminação)
- **Baixo custo:** Sigfox (monitoramento básico)

---

## 📋 CONFIGURAÇÕES TESTADAS

### **📍 Distâncias:**
- 3.000m, 5.000m, 10.000m, 15.000m, 30.000m, 50.000m

### **📱 Densidades de Dispositivos:**
- 10, 100, 500, 1.000, 5.000, 10.000, 30.000, 50.000

### **📊 Métricas Coletadas:**
- PDR (%), PLR (%), Vazão (bps), Atraso (s)
- Consumo Energia (J), Utilização do Canal (%)
- Banda Utilizada (kHz), Retransmissão (%)

---

## 🔧 REPRODUZIBILIDADE

### **📊 Dados Disponíveis:**
- **CSV principal:** `resultados/resultados_extendidos_completo.csv`
- **CSVs por tecnologia:** Sigfox, LoRaWAN, NB-IoT
- **Dados brutos:** `resultados/resultados_extendidos/`

### **🔍 Scripts de Análise:**
- **Análise simples:** `analise/analise_simples.py`
- **Análise completa:** `analise/analise_completa.py`
- **Conversão:** `scripts/converter_extendidos_csv.py`

### **📖 Documentação:**
- **Guia completo:** `documentacao/GUIA_COMPLETO_EXPERIMENTOS_LPWAN.md`
- **Metodologia:** `documentacao/METODOLOGIA_SIMULACAO_LPWAN.md`
- **Relatório:** `documentacao/RELATORIO_ORIENTADOR.md`

---

## 🎓 VALOR ACADÊMICO

### **✅ Robustez Estatística:**
- **1.440 experimentos** (estatisticamente significativo)
- **144 configurações únicas** (cobertura abrangente)
- **10 execuções por configuração** (confiabilidade)

### **✅ Metodologia Válida:**
- Simulação baseada em NS-3
- Parâmetros realistas
- Métricas padronizadas
- Reproduzibilidade garantida

### **✅ Aplicabilidade:**
- Resultados práticos para escolha de tecnologias
- Recomendações baseadas em dados
- Insights para implementação real

---

## 📞 CONTATO E SUPORTE

### **📧 Para Dúvidas:**
- Revisar `analise/GUIA_ANALISE_RESULTADOS.md`
- Consultar `documentacao/GUIA_COMPLETO_EXPERIMENTOS_LPWAN.md`
- Verificar `documentacao/METODOLOGIA_SIMULACAO_LPWAN.md`

### **🔧 Para Problemas Técnicos:**
- Verificar dependências Python (pandas, matplotlib, seaborn)
- Executar `scripts/verificar_instalacao.sh`
- Consultar logs de erro

---

## 📄 LICENÇA

Este projeto foi desenvolvido para fins acadêmicos e de pesquisa. Os resultados são baseados em simulações e devem ser validados em implementações reais.

---

*Projeto LPWAN - Análise Comparativa de Tecnologias*  
*Data: 2025*  
*Tecnologias: Sigfox, LoRaWAN, NB-IoT*  
*Total de Experimentos: 1.440* 
