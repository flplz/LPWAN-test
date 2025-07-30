#!/usr/bin/env python3
"""
Análise Completa dos Resultados LPWAN
Combina análise estatística, geração de gráficos e relatório
"""

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os
from datetime import datetime

def carregar_dados():
    """Carrega os dados dos experimentos"""
    print("📊 Carregando dados...")
    df = pd.read_csv('resultados/resultados_extendidos_completo.csv')
    print(f"✅ Dados carregados: {len(df)} registros")
    return df

def analise_estatistica(df):
    """Realiza análise estatística completa"""
    print("\n" + "="*60)
    print("📈 ANÁLISE ESTATÍSTICA COMPLETA")
    print("="*60)
    
    # Estatísticas gerais
    print(f"📊 Total de experimentos: {len(df)}")
    print(f"📡 Tecnologias: {list(df['Tecnologia'].unique())}")
    print(f"📍 Distâncias: {sorted(df['Distância (m)'].unique())}")
    print(f"📱 Dispositivos: {sorted(df['Dispositivos'].unique())}")
    
    # Análise por tecnologia
    print("\n🔬 COMPARAÇÃO DETALHADA POR TECNOLOGIA:")
    for tech in df['Tecnologia'].unique():
        tech_data = df[df['Tecnologia'] == tech]
        print(f"\n📡 {tech}:")
        print(f"   • PDR médio: {tech_data['PDR (%)'].mean():.1f}% ± {tech_data['PDR (%)'].std():.1f}%")
        print(f"   • Vazão média: {tech_data['Vazão (bps)'].mean():.0f} ± {tech_data['Vazão (bps)'].std():.0f} bps")
        print(f"   • Consumo energético: {tech_data['Consumo Energia (J)'].mean():.1f} ± {tech_data['Consumo Energia (J)'].std():.1f} J")
        print(f"   • Atraso médio: {tech_data['Atraso Médio (s)'].mean():.3f} ± {tech_data['Atraso Médio (s)'].std():.3f} s")
        print(f"   • Utilização do canal: {tech_data['Utilização do Canal (%)'].mean():.1f}%")
        print(f"   • Banda utilizada: {tech_data['Banda Utilizada (kHz)'].mean():.0f} kHz")

def gerar_graficos_avancados(df):
    """Gera gráficos avançados"""
    print("\n📊 Gerando gráficos avançados...")
    
    # Configurar estilo
    plt.style.use('default')
    
    # 1. Gráfico principal com 4 subplots
    fig, axes = plt.subplots(2, 2, figsize=(16, 12))
    fig.suptitle('Análise Comparativa LPWAN - 1.440 Experimentos', fontsize=16, fontweight='bold')
    
    cores = {'NBIOT': '#FF6B6B', 'LORAWAN': '#4ECDC4', 'SIGFOX': '#45B7D1'}
    
    # PDR vs Distância
    ax1 = axes[0, 0]
    for tech in ['NBIOT', 'LORAWAN', 'SIGFOX']:
        tech_data = df[df['Tecnologia'] == tech]
        ax1.scatter(tech_data['Distância (m)'], tech_data['PDR (%)'], 
                   alpha=0.6, label=tech, s=30, color=cores[tech])
    ax1.set_xlabel('Distância (m)')
    ax1.set_ylabel('PDR (%)')
    ax1.set_title('PDR vs Distância')
    ax1.legend()
    ax1.grid(True, alpha=0.3)
    
    # Vazão vs Distância
    ax2 = axes[0, 1]
    for tech in ['NBIOT', 'LORAWAN', 'SIGFOX']:
        tech_data = df[df['Tecnologia'] == tech]
        ax2.scatter(tech_data['Distância (m)'], tech_data['Vazão (bps)'], 
                   alpha=0.6, label=tech, s=30, color=cores[tech])
    ax2.set_xlabel('Distância (m)')
    ax2.set_ylabel('Vazão (bps)')
    ax2.set_title('Vazão vs Distância')
    ax2.legend()
    ax2.grid(True, alpha=0.3)
    
    # Consumo vs Dispositivos
    ax3 = axes[1, 0]
    for tech in ['NBIOT', 'LORAWAN', 'SIGFOX']:
        tech_data = df[df['Tecnologia'] == tech]
        ax3.scatter(tech_data['Dispositivos'], tech_data['Consumo Energia (J)'], 
                   alpha=0.6, label=tech, s=30, color=cores[tech])
    ax3.set_xlabel('Número de Dispositivos')
    ax3.set_ylabel('Consumo Energia (J)')
    ax3.set_title('Consumo de Energia vs Dispositivos')
    ax3.legend()
    ax3.grid(True, alpha=0.3)
    
    # Atraso vs Dispositivos
    ax4 = axes[1, 1]
    for tech in ['NBIOT', 'LORAWAN', 'SIGFOX']:
        tech_data = df[df['Tecnologia'] == tech]
        ax4.scatter(tech_data['Dispositivos'], tech_data['Atraso Médio (s)'], 
                   alpha=0.6, label=tech, s=30, color=cores[tech])
    ax4.set_xlabel('Número de Dispositivos')
    ax4.set_ylabel('Atraso Médio (s)')
    ax4.set_title('Atraso vs Dispositivos')
    ax4.legend()
    ax4.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('analise_completa_graficos.png', dpi=300, bbox_inches='tight')
    print("✅ Gráfico principal salvo como 'analise_completa_graficos.png'")
    
    # 2. Gráfico de radar
    fig2, ax = plt.subplots(figsize=(12, 8))
    
    metricas = ['PDR (%)', 'Vazão (bps)', 'Consumo Energia (J)', 'Atraso Médio (s)']
    tecnologias = ['NBIOT', 'LORAWAN', 'SIGFOX']
    
    dados_norm = []
    for metrica in metricas:
        valores = []
        for tech in tecnologias:
            tech_data = df[df['Tecnologia'] == tech]
            if metrica == 'Consumo Energia (J)' or metrica == 'Atraso Médio (s)':
                valor = 1 - (tech_data[metrica].mean() / tech_data[metrica].max())
            else:
                valor = tech_data[metrica].mean() / tech_data[metrica].max()
            valores.append(valor)
        dados_norm.append(valores)
    
    angles = np.linspace(0, 2 * np.pi, len(metricas), endpoint=False).tolist()
    dados_norm = np.array(dados_norm)
    
    for i, tech in enumerate(tecnologias):
        valores = dados_norm[:, i]
        valores = np.concatenate((valores, [valores[0]]))
        angulos = angles + [angles[0]]
        ax.plot(angulos, valores, 'o-', linewidth=2, label=tech, color=cores[tech])
        ax.fill(angulos, valores, alpha=0.25, color=cores[tech])
    
    ax.set_xticks(angles)
    ax.set_xticklabels(metricas)
    ax.set_ylim(0, 1)
    ax.set_title('Comparação Normalizada das Tecnologias LPWAN')
    ax.legend()
    ax.grid(True)
    
    plt.tight_layout()
    plt.savefig('analise_completa_radar.png', dpi=300, bbox_inches='tight')
    print("✅ Gráfico de radar salvo como 'analise_completa_radar.png'")

def gerar_relatorio_final(df):
    """Gera relatório final em markdown"""
    print("\n📝 Gerando relatório final...")
    
    # Calcular estatísticas
    pdr_medio = df.groupby('Tecnologia')['PDR (%)'].mean()
    vazao_medio = df.groupby('Tecnologia')['Vazão (bps)'].mean()
    energia_medio = df.groupby('Tecnologia')['Consumo Energia (J)'].mean()
    atraso_medio = df.groupby('Tecnologia')['Atraso Médio (s)'].mean()
    
    relatorio = f"""# 📊 RELATÓRIO FINAL - ANÁLISE LPWAN

## 🎯 RESUMO EXECUTIVO

**Data da Análise:** {datetime.now().strftime('%d/%m/%Y %H:%M')}  
**Total de Experimentos:** {len(df)}  
**Tecnologias Analisadas:** {', '.join(df['Tecnologia'].unique())}  
**Configurações:** {len(df.groupby(['Distância (m)', 'Dispositivos']))} configurações únicas  

## 📈 RESULTADOS PRINCIPAIS

### 🏆 MELHORES TECNOLOGIAS POR MÉTRICA

| Métrica | Melhor Tecnologia | Valor |
|---------|------------------|-------|
| **PDR (%)** | **{pdr_medio.idxmax()}** | **{pdr_medio.max():.1f}%** |
| **Vazão (bps)** | **{vazao_medio.idxmax()}** | **{vazao_medio.max():.0f} bps** |
| **Eficiência Energética** | **{energia_medio.idxmin()}** | **{energia_medio.min():.1f} J** |
| **Menor Atraso** | **{atraso_medio.idxmin()}** | **{atraso_medio.min():.3f} s** |

## 🔬 ANÁLISE DETALHADA

### 📡 {df['Tecnologia'].unique()[0]}
- **PDR médio:** {pdr_medio.iloc[0]:.1f}%
- **Vazão média:** {vazao_medio.iloc[0]:.0f} bps
- **Consumo energético:** {energia_medio.iloc[0]:.1f} J
- **Atraso médio:** {atraso_medio.iloc[0]:.3f} s

### 📡 {df['Tecnologia'].unique()[1]}
- **PDR médio:** {pdr_medio.iloc[1]:.1f}%
- **Vazão média:** {vazao_medio.iloc[1]:.0f} bps
- **Consumo energético:** {energia_medio.iloc[1]:.1f} J
- **Atraso médio:** {atraso_medio.iloc[1]:.3f} s

### 📡 {df['Tecnologia'].unique()[2]}
- **PDR médio:** {pdr_medio.iloc[2]:.1f}%
- **Vazão média:** {vazao_medio.iloc[2]:.0f} bps
- **Consumo energético:** {energia_medio.iloc[2]:.1f} J
- **Atraso médio:** {atraso_medio.iloc[2]:.3f} s

## 📊 CONFIGURAÇÕES TESTADAS

- **Distâncias:** {sorted(df['Distância (m)'].unique())}
- **Densidades:** {sorted(df['Dispositivos'].unique())}
- **Execuções:** 10 por configuração

## 🎨 VISUALIZAÇÕES GERADADAS

1. `analise_completa_graficos.png` - Gráficos comparativos principais
2. `analise_completa_radar.png` - Gráfico de radar normalizado

## ✅ CONCLUSÕES

Baseado em {len(df)} experimentos, as principais conclusões são:

1. **{pdr_medio.idxmax()}** apresenta a melhor confiabilidade (PDR: {pdr_medio.max():.1f}%)
2. **{energia_medio.idxmin()}** é a mais eficiente energeticamente ({energia_medio.min():.1f} J)
3. **{vazao_medio.idxmax()}** oferece a maior capacidade de transmissão
4. **{atraso_medio.idxmin()}** apresenta a menor latência

---

*Relatório gerado automaticamente em {datetime.now().strftime('%d/%m/%Y às %H:%M')}*
"""
    
    with open('RELATORIO_FINAL_ANALISE.md', 'w', encoding='utf-8') as f:
        f.write(relatorio)
    
    print("✅ Relatório final salvo como 'RELATORIO_FINAL_ANALISE.md'")

def main():
    """Função principal"""
    print("🚀 ANÁLISE COMPLETA DOS RESULTADOS LPWAN")
    print("="*60)
    
    # Carregar dados
    df = carregar_dados()
    
    # Análise estatística
    analise_estatistica(df)
    
    # Gerar gráficos
    gerar_graficos_avancados(df)
    
    # Gerar relatório
    gerar_relatorio_final(df)
    
    print("\n" + "="*60)
    print("🎉 ANÁLISE COMPLETA CONCLUÍDA!")
    print("📁 Arquivos gerados:")
    print("   • analise_completa_graficos.png - Gráficos principais")
    print("   • analise_completa_radar.png - Gráfico de radar")
    print("   • RELATORIO_FINAL_ANALISE.md - Relatório completo")
    print("="*60)

if __name__ == "__main__":
    main() 