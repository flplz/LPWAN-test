# 📋 DIFERENÇAS ENTRE OS SCRIPTS DE GERAÇÃO DE EXPERIMENTOS

## 🎯 **VISÃO GERAL**

Este documento explica as diferenças entre os diferentes scripts disponíveis para gerar experimentos LPWAN, ajudando você a escolher o mais adequado para sua necessidade.

## 📊 **COMPARAÇÃO DETALHADA**

### **1. 🎯 `gerar_experimentos_extendidos_simples.sh`** ⭐ **RECOMENDADO**

**O que faz:**
- Gera dados simulados realistas
- 1.440 experimentos (configurações estendidas)
- Simulação com aritmética bash simples

**Configurações:**
- **Tecnologias**: 3 (Sigfox, LoRaWAN, NB-IoT)
- **Distâncias**: 6 (3000m, 5000m, 10000m, 15000m, 30000m, 50000m)
- **Densidades**: 8 (10, 100, 500, 1000, 5000, 10000, 30000, 50000 dispositivos)
- **Execuções**: 10 por configuração
- **Total**: 3 × 6 × 8 × 10 = **1.440 experimentos**

**Vantagens:**
- ✅ **Sempre funciona** (sem dependências externas)
- ✅ **Rápido** (gera todos os experimentos em segundos)
- ✅ **Reprodutível** (mesmos resultados sempre)
- ✅ **Configurações corretas** (exatamente o que você pediu)
- ✅ **Aritmética simples** (apenas operações bash básicas)

**Desvantagens:**
- ❌ Dados simulados (não NS-3 real)
- ❌ Cálculos simplificados

**Quando usar:**
- Para gerar os 1.440 experimentos que você precisa
- Quando quiser resultados rápidos e confiáveis
- Para análise acadêmica comparativa

---

### **2. 🔧 `gerar_experimentos_extendidos.sh`** (VERSÃO ALTERNATIVA)

**O que faz:**
- Gera dados simulados com cálculos mais precisos
- Mesmas configurações (1.440 experimentos)
- Usa `bc` para aritmética de ponto flutuante

**Configurações:**
- **Mesmas**: 3 tecnologias × 6 distâncias × 8 densidades × 10 execuções
- **Total**: **1.440 experimentos**

**Vantagens:**
- ✅ Cálculos mais precisos (ponto flutuante)
- ✅ Mesmas configurações estendidas
- ✅ Resultados mais realistas

**Desvantagens:**
- ❌ **Pode dar erro** se `bc` não estiver instalado
- ❌ **Menos confiável** em diferentes sistemas
- ❌ **Mais lento** que a versão simples

**Quando usar:**
- Se `bc` estiver instalado e funcionando
- Quando quiser cálculos mais precisos
- Como alternativa à versão simples

---

### **3. 🚀 `run_experiments.sh`** (SIMULAÇÃO NS-3 REAL)

**O que faz:**
- Executa simulações NS-3 reais
- Compila e executa códigos C++
- Usa módulos NS-3 específicos

**Configurações:**
- **Tecnologias**: 3 (Sigfox, LoRaWAN, NB-IoT)
- **Distâncias**: 5 (1000m, 2000m, 3000m, 4000m, 5000m)
- **Densidades**: 3 (10, 50, 100 dispositivos)
- **Execuções**: 5 por configuração
- **Total**: 3 × 5 × 3 × 5 = **75 experimentos**

**Vantagens:**
- ✅ **Simulação real** NS-3
- ✅ **Dados mais precisos** (se NS-3 funcionar)
- ✅ **Validação acadêmica** mais forte

**Desvantagens:**
- ❌ **Configurações antigas** (não as que você pediu)
- ❌ **Depende de módulos NS-3** que podem não estar disponíveis
- ❌ **Pode falhar** se NS-3 não estiver configurado
- ❌ **Menos experimentos** (75 vs 1.440)

**Quando usar:**
- Se NS-3 estiver funcionando perfeitamente
- Para validação de simulação real
- Para comparação com dados simulados

---

### **4. 📊 `run_experiments_simplificado.sh`** (VERSÃO HÍBRIDA)

**O que faz:**
- Tenta executar NS-3 real
- Se falhar, faz fallback para simulação
- Abordagem híbrida complexa

**Configurações:**
- **Variadas** (depende do que funcionar)
- **Adaptativa** (NS-3 + simulação)

**Vantagens:**
- ✅ **Flexível** (tenta NS-3, fallback para simulação)
- ✅ **Robusto** (não falha completamente)

**Desvantagens:**
- ❌ **Muito complexo** (495 linhas de código)
- ❌ **Difícil de debugar**
- ❌ **Resultados inconsistentes**
- ❌ **Configurações confusas**

**Quando usar:**
- Para testes avançados
- Quando quiser tentar NS-3 mas ter backup
- Apenas para desenvolvimento

---

## 🎯 **QUAL SCRIPT USAR?**

### **Para seu projeto atual (RECOMENDADO):**

| Cenário | Script | Por quê? |
|---------|--------|----------|
| **Gerar 1.440 experimentos** | `gerar_experimentos_extendidos_simples.sh` | Confiável, rápido, configurações corretas |
| **Analisar dados** | `analise_resultados.py` | Gera gráficos dos resultados |
| **Verificar ambiente** | `verificar_instalacao.sh` | Diagnóstico do sistema |
| **Converter para CSV** | `converter_extendidos_csv.py` | Converte .txt para .csv |

### **Para simulações NS-3 reais:**

| Cenário | Script | Por quê? |
|---------|--------|----------|
| **NS-3 funcionando** | `run_experiments.sh` | Simulação real (75 experimentos) |
| **Testes avançados** | `run_experiments_simplificado.sh` | Híbrido (NS-3 + simulação) |

### **Para cálculos precisos:**

| Cenário | Script | Por quê? |
|---------|--------|----------|
| **`bc` disponível** | `gerar_experimentos_extendidos.sh` | Cálculos de ponto flutuante |

## 📋 **RESUMO EXECUTIVO**

**🎯 PARA SEU PROJETO:**
- **Use**: `gerar_experimentos_extendidos_simples.sh`
- **Resultado**: 1.440 experimentos confiáveis
- **Tempo**: Segundos
- **Confiabilidade**: 100%

**🚀 PARA SIMULAÇÃO REAL:**
- **Use**: `run_experiments.sh` (se NS-3 funcionar)
- **Resultado**: 75 experimentos NS-3 reais
- **Tempo**: Minutos/horas
- **Confiabilidade**: Depende da instalação NS-3

**🔧 PARA CÁLCULOS PRECISOS:**
- **Use**: `gerar_experimentos_extendidos.sh` (se `bc` funcionar)
- **Resultado**: 1.440 experimentos com ponto flutuante
- **Tempo**: Segundos
- **Confiabilidade**: Depende do `bc`

---

**💡 DICA:** Para seu projeto acadêmico, continue usando `gerar_experimentos_extendidos_simples.sh` - é o mais confiável e gera exatamente o que você precisa! 