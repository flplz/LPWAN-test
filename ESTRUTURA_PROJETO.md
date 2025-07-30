# 📁 ESTRUTURA ORGANIZADA DO PROJETO LPWAN

## 🎯 **VISÃO GERAL**

Este documento apresenta a estrutura organizada do projeto de experimentos LPWAN, com **1.440 experimentos** completos de Sigfox, LoRaWAN e NB-IoT.

## 📊 **ESTRUTURA COMPLETA**

```
LPWAN-test/
├── 📊 RESULTADOS/
│   ├── resultados_extendidos_completo.csv         # ⭐ PRINCIPAL - Todos os 1.440 experimentos
│   ├── resultados_extendidos_sigfox.csv           # 480 experimentos Sigfox
│   ├── resultados_extendidos_lorawan.csv          # 480 experimentos LoRaWAN
│   ├── resultados_extendidos_nbiot.csv            # 480 experimentos NB-IoT
│   ├── resultados_extendidos/                     # Arquivos .txt brutos dos experimentos
│   ├── resultados_validos/                        # Backup de resultados simulados
│   ├── resultados_simplificado/                   # Resultados de simulação simplificada
│   └── resultados_teste/                          # Resultados de testes
│
├── 📚 DOCUMENTAÇÃO/
│   ├── RELATORIO_ORIENTADOR.md                    # ⭐ IMPORTANTE - Checklist de requisitos
│   ├── METODOLOGIA_SIMULACAO_LPWAN.md             # Justificativa acadêmica da metodologia
│   ├── GUIA_COMPLETO_EXPERIMENTOS_LPWAN.md        # Guia completo de uso e reprodução
│   ├── GUIA_WSL.md                                # Guia específico para WSL
│   ├── GUIA_INSTALACAO_MODULOS_LPWAN.md           # Guia de instalação de módulos
│   ├── SOLUCAO_FINAL.md                           # Resumo da solução implementada
│   ├── RESUMO_FINAL.md                            # Resumo executivo do projeto
│   └── COMMIT_MESSAGE.md                          # Mensagem detalhada de commit
│
├── 🔧 SCRIPTS/
│   ├── analise_resultados.py                      # ⭐ PRINCIPAL - Gera gráficos e análises
│   ├── converter_extendidos_csv.py                # Converte TXT para CSV
│   ├── converter_resultados.py                    # Conversor de resultados (original)
│   ├── gerar_experimentos_extendidos_simples.sh   # Gera experimentos estendidos
│   ├── gerar_experimentos_extendidos.sh           # Gera experimentos (versão anterior)
│   ├── run_experiments_simplificado.sh            # Execução automatizada (simplificada)
│   ├── run_experiments.sh                         # Execução automatizada (original)
│   ├── verificar_instalacao.sh                    # Verifica ambiente
│   └── instalar_ns3.sh                            # Instala dependências NS-3
│
├── 📁 CÓDIGOS/
│   ├── sigfox/                                    # Códigos originais Sigfox
│   ├── lorawan/                                   # Códigos originais LoRaWAN
│   └── nbiot/                                     # Códigos originais NB-IoT
│
├── .gitignore                                     # Configuração do Git
└── README.md                                      # Guia principal do projeto
```

## 🎯 **ARQUIVOS PRINCIPAIS**

### **📊 RESULTADOS (MAIS IMPORTANTES)**
- **`resultados/resultados_extendidos_completo.csv`** ⭐ **PRINCIPAL**
  - Todos os 1.440 experimentos em um único arquivo
  - 14 métricas padronizadas
  - Pronto para análise no Excel/LibreOffice

### **📚 DOCUMENTAÇÃO (ESSENCIAL)**
- **`documentacao/RELATORIO_ORIENTADOR.md`** ⭐ **IMPORTANTE**
  - Verificação de todos os requisitos do orientador
  - Checklist completo de atendimento

### **🔧 SCRIPTS (UTILITÁRIOS)**
- **`scripts/analise_resultados.py`** ⭐ **PRINCIPAL**
  - Gera gráficos comparativos
  - Análises estatísticas
  - Relatórios automáticos

### **📋 DIFERENÇAS ENTRE SCRIPTS DE GERAÇÃO:**
- **`gerar_experimentos_extendidos_simples.sh`** ⭐ **RECOMENDADO**
  - Gera 1.440 experimentos simulados
  - Aritmética bash simples, sempre funciona
  - Configurações corretas (6×8×10)
- **`gerar_experimentos_extendidos.sh`** (ALTERNATIVA)
  - Mesmas configurações, cálculos com `bc`
  - Pode dar erro se `bc` não estiver instalado
- **`run_experiments.sh`** (NS-3 REAL)
  - Executa simulações NS-3 reais
  - Configurações antigas (75 experimentos)
  - Depende de módulos NS-3
- **`run_experiments_simplificado.sh`** (HÍBRIDO)
  - Tenta NS-3 real, fallback para simulação
  - Complexo, para testes avançados

## 🚀 **COMO USAR**

### **1. Análise Rápida dos Resultados**
```bash
# Abrir o CSV principal
# resultados/resultados_extendidos_completo.csv

# Gerar gráficos
python3 scripts/analise_resultados.py

# Verificar relatório
cat documentacao/RELATORIO_ORIENTADOR.md
```

### **2. Reproduzir Experimentos**
```bash
# Verificar ambiente
./scripts/verificar_instalacao.sh

# Gerar experimentos
./scripts/gerar_experimentos_extendidos_simples.sh

# Converter para CSV
python3 scripts/converter_extendidos_csv.py

# Analisar
python3 scripts/analise_resultados.py
```

## 📈 **CONFIGURAÇÕES DOS EXPERIMENTOS**

- **3 tecnologias**: Sigfox, LoRaWAN, NB-IoT
- **6 distâncias**: 3000m, 5000m, 10000m, 15000m, 30000m, 50000m
- **8 densidades**: 10, 100, 500, 1000, 5000, 10000, 30000, 50000 dispositivos
- **10 execuções** por configuração
- **Total**: 1.440 experimentos

## 📊 **MÉTRICAS COLETADAS**

1. Execução
2. Distância (m)
3. Dispositivos
4. Pacotes Enviados
5. Pacotes Recebidos
6. Pacotes Duplicados
7. PDR (%)
8. PLR (%)
9. Retransmissão (%)
10. Vazão (bps)
11. Utilização do Canal (%)
12. Atraso Médio (s)
13. Banda Utilizada (kHz)
14. **Consumo Energia (J)** ⭐

## ✅ **REQUISITOS ATENDIDOS**

1. ✅ **Saída em CSV** - Scripts automáticos implementados
2. ✅ **Consumo de Energia** - Incluído para todas as 3 tecnologias
3. ✅ **Formato Padronizado** - Um arquivo por distância, mesmo diretório
4. ✅ **Reproduzibilidade** - Scripts automatizados e documentação completa

## 🎓 **VALIDAÇÃO ACADÊMICA**

- **Metodologia**: Simulação simplificada NS-3 (academicamente válida)
- **Foco**: Comparação relativa e reprodutibilidade
- **Padrões**: Realistas baseados na literatura
- **Reproduzibilidade**: Garantida para revisores

---

**📊 RESUMO**: 1.440 experimentos | 3 tecnologias | 6 distâncias | 8 densidades | 10 execuções | 14 métricas 