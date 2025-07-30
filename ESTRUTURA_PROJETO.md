# 📁 ESTRUTURA ORGANIZADA DO PROJETO LPWAN

## 🎯 VISÃO GERAL

Este documento descreve a **estrutura organizada e limpa** do projeto de análise comparativa de tecnologias LPWAN. O projeto foi reorganizado para máxima clareza e facilidade de uso.

---

## 📂 ESTRUTURA PRINCIPAL

```
LPWAN-test/
├── 📊 analise/                    # Scripts e relatórios de análise
│   ├── analise_simples.py         # Análise rápida dos dados
│   ├── analise_completa.py        # Análise completa + gráficos + relatório
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
│   └── resultados_extendidos/     # Dados brutos (.txt)
├── 🔧 scripts/                    # Scripts de automação
│   ├── gerar_experimentos_extendidos_simples.sh
│   ├── converter_extendidos_csv.py
│   ├── instalar_ns3.sh
│   └── verificar_instalacao.sh
├── 💻 codigos/                    # Códigos das simulações
├── 📖 README.md                   # Documentação principal
├── 📋 ESTRUTURA_PROJETO.md       # Este arquivo
└── 🚫 .gitignore                  # Arquivos ignorados pelo Git
```

---

## 📊 DETALHAMENTO POR PASTA

### **📊 `analise/` - Scripts e Relatórios de Análise**

**Propósito:** Contém todos os scripts e relatórios relacionados à análise dos dados.

#### **Scripts Python:**
- **`analise_simples.py`** - Análise básica e estatísticas gerais
- **`analise_completa.py`** - Análise detalhada + gráficos + relatório automático

#### **Gráficos Gerados:**
- **`analise_completa_graficos.png`** - 4 gráficos comparativos principais
- **`analise_completa_radar.png`** - Gráfico de radar normalizado
- **`graficos_lpwan.png`** - Versão alternativa dos gráficos

#### **Relatórios:**
- **`GUIA_ANALISE_RESULTADOS.md`** - Guia completo de como analisar
- **`RELATORIO_FINAL_ANALISE.md`** - Relatório principal atualizado
- **`RESUMO_ANALISE_FINAL.md`** - Resumo executivo dos resultados

### **📋 `documentacao/` - Documentação Técnica**

**Propósito:** Contém toda a documentação técnica e metodológica do projeto.

#### **Guias e Metodologia:**
- **`GUIA_COMPLETO_EXPERIMENTOS_LPWAN.md`** - Guia completo de execução
- **`METODOLOGIA_SIMULACAO_LPWAN.md`** - Justificativa metodológica
- **`DIFERENCAS_SCRIPTS.md`** - Explicação das diferenças entre scripts

#### **Relatórios:**
- **`RELATORIO_ORIENTADOR.md`** - Relatório para o orientador
- **`RESUMO_FINAL.md`** - Resumo executivo do projeto

### **📈 `resultados/` - Dados dos Experimentos**

**Propósito:** Contém todos os dados gerados pelos experimentos.

#### **Arquivos CSV Principais:**
- **`resultados_extendidos_completo.csv`** - ⭐ **PRINCIPAL** (1.440 experimentos)
- **`resultados_extendidos_sigfox.csv`** - Dados apenas Sigfox (480 experimentos)
- **`resultados_extendidos_lorawan.csv`** - Dados apenas LoRaWAN (480 experimentos)
- **`resultados_extendidos_nbiot.csv`** - Dados apenas NB-IoT (480 experimentos)

#### **Dados Brutos:**
- **`resultados_extendidos/`** - Arquivos .txt brutos (1.440 arquivos)

### **🔧 `scripts/` - Scripts de Automação**

**Propósito:** Contém scripts para automação de tarefas.

#### **Geração de Experimentos:**
- **`gerar_experimentos_extendidos_simples.sh`** - ⭐ **PRINCIPAL** (1.440 experimentos)

#### **Conversão e Análise:**
- **`converter_extendidos_csv.py`** - Converte .txt para CSV
- **`verificar_instalacao.sh`** - Verifica ambiente do sistema
- **`instalar_ns3.sh`** - Instala NS-3 (se necessário)

### **💻 `codigos/` - Códigos das Simulações**

**Propósito:** Contém os códigos originais das simulações NS-3.

#### **Estrutura:**
- **`sigfox/`** - Códigos Sigfox
- **`lorawan/`** - Códigos LoRaWAN  
- **`nbiot/`** - Códigos NB-IoT

---

## 🎯 FLUXO DE TRABALHO RECOMENDADO

### **1. 📊 Análise dos Resultados (Mais Comum)**
```bash
cd analise
python3 analise_simples.py          # Análise rápida
python3 analise_completa.py         # Análise completa
```

### **2. 🔄 Reprodução dos Experimentos**
```bash
./scripts/gerar_experimentos_extendidos_simples.sh
python3 scripts/converter_extendidos_csv.py
cd analise
python3 analise_completa.py
```

### **3. 📖 Leitura da Documentação**
- **`analise/RELATORIO_FINAL_ANALISE.md`** - Relatório principal
- **`analise/GUIA_ANALISE_RESULTADOS.md`** - Como analisar
- **`documentacao/RELATORIO_ORIENTADOR.md`** - Para orientador

---

## 📋 ARQUIVOS PRINCIPAIS

### **⭐ Arquivos Mais Importantes:**

1. **`resultados/resultados_extendidos_completo.csv`** - Dados principais
2. **`analise/analise_completa.py`** - Script de análise principal
3. **`analise/RELATORIO_FINAL_ANALISE.md`** - Relatório principal
4. **`README.md`** - Documentação geral
5. **`scripts/gerar_experimentos_extendidos_simples.sh`** - Geração de dados

### **📊 Arquivos de Análise:**
- **`analise/analise_simples.py`** - Análise básica
- **`analise/analise_completa.py`** - Análise completa
- **`analise/*.png`** - Gráficos gerados

### **📖 Arquivos de Documentação:**
- **`analise/GUIA_ANALISE_RESULTADOS.md`** - Guia de análise
- **`analise/RESUMO_ANALISE_FINAL.md`** - Resumo executivo
- **`documentacao/GUIA_COMPLETO_EXPERIMENTOS_LPWAN.md`** - Guia completo
- **`documentacao/METODOLOGIA_SIMULACAO_LPWAN.md`** - Metodologia

---

## 🧹 LIMPEZA REALIZADA

### **🗑️ Arquivos Removidos:**
- `analise_rapida.py` (substituído por `analise_completa.py`)
- `gerar_graficos.py` (funcionalidade integrada)
- `analise_resultados.png` (versões melhores disponíveis)
- `comparacao_radar.png` (duplicado)
- `RELATORIO_RESULTADOS.md` (substituído por versão melhor)
- `h --force-with-lease` (arquivo temporário Git)
- Scripts antigos e desnecessários
- Documentação redundante

### **📁 Reorganização:**
- **Criação da pasta `analise/`** para scripts e relatórios de análise
- **Limpeza da pasta `resultados/`** (removidos dados de teste)
- **Limpeza da pasta `scripts/`** (removidos scripts antigos)
- **Limpeza da pasta `documentacao/`** (removida documentação redundante)

---

## ✅ BENEFÍCIOS DA REORGANIZAÇÃO

### **🎯 Clareza:**
- Estrutura lógica e intuitiva
- Separação clara de responsabilidades
- Fácil navegação

### **📊 Facilidade de Uso:**
- Scripts organizados por função
- Documentação centralizada
- Fluxo de trabalho claro

### **🔧 Manutenibilidade:**
- Arquivos relacionados agrupados
- Fácil localização de recursos
- Estrutura escalável

### **📖 Profissionalismo:**
- Organização acadêmica padrão
- Documentação completa
- Reproduzibilidade garantida

---

## 🚀 PRÓXIMOS PASSOS

### **Para Usuários:**
1. **Ler `README.md`** para visão geral
2. **Executar `analise/analise_completa.py`** para análise
3. **Revisar relatórios** em `analise/`
4. **Consultar documentação** conforme necessário

### **Para Desenvolvedores:**
1. **Manter estrutura** organizada
2. **Documentar mudanças** adequadamente
3. **Testar scripts** antes de commitar
4. **Atualizar documentação** quando necessário

---

*Estrutura organizada para máxima clareza e facilidade de uso*  
*Data: 2024*  
*Total de arquivos organizados: ~20 arquivos principais* 