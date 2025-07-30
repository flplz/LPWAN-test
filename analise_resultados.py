#!/usr/bin/env python3
"""
Script para análise e geração de gráficos dos resultados dos experimentos LPWAN
Autor: Assistente IA
Data: 2024
"""

import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
import glob
import os

# Configurar estilo dos gráficos
plt.style.use('seaborn-v0_8')
sns.set_palette("husl")

def carregar_dados():
    """Carrega todos os CSVs gerados"""
    dados = {}
    
    # Procurar por arquivos CSV
    csv_files = glob.glob("*.csv")
    
    for file in csv_files:
        try:
            df = pd.read_csv(file)
            if not df.empty:
                # Extrair nome da tecnologia do nome do arquivo
                if 'sigfox' in file.lower():
                    dados['sigfox'] = df
                elif 'lorawan' in file.lower():
                    dados['lorawan'] = df
                elif 'nbiot' in file.lower():
                    dados['nbiot'] = df
                print(f"Carregado: {file} - {len(df)} registros")
        except Exception as e:
            print(f"Erro ao carregar {file}: {e}")
    
    return dados

def gerar_graficos_comparativos(dados):
    """Gera gráficos comparativos entre as tecnologias"""
    
    if not dados:
        print("Nenhum dado encontrado para análise")
        return
    
    # Configurar figura
    fig, axes = plt.subplots(2, 3, figsize=(18, 12))
    fig.suptitle('Comparação de Tecnologias LPWAN', fontsize=16, fontweight='bold')
    
    # Cores para cada tecnologia
    cores = {'sigfox': '#FF6B6B', 'lorawan': '#4ECDC4', 'nbiot': '#45B7D1'}
    
    # 1. PDR vs Distância
    ax1 = axes[0, 0]
    for tech, df in dados.items():
        if not df.empty:
            sns.boxplot(data=df, x='Distância (m)', y='PDR (%)', ax=ax1, color=cores[tech], alpha=0.7)
    ax1.set_title('PDR vs Distância', fontweight='bold')
    ax1.set_xlabel('Distância (m)')
    ax1.set_ylabel('PDR (%)')
    
    # 2. Vazão vs Distância
    ax2 = axes[0, 1]
    for tech, df in dados.items():
        if not df.empty:
            sns.boxplot(data=df, x='Distância (m)', y='Vazão (bps)', ax=ax2, color=cores[tech], alpha=0.7)
    ax2.set_title('Vazão vs Distância', fontweight='bold')
    ax2.set_xlabel('Distância (m)')
    ax2.set_ylabel('Vazão (bps)')
    
    # 3. Atraso vs Distância
    ax3 = axes[0, 2]
    for tech, df in dados.items():
        if not df.empty:
            sns.boxplot(data=df, x='Distância (m)', y='Atraso Médio (s)', ax=ax3, color=cores[tech], alpha=0.7)
    ax3.set_title('Atraso vs Distância', fontweight='bold')
    ax3.set_xlabel('Distância (m)')
    ax3.set_ylabel('Atraso Médio (s)')
    
    # 4. Consumo de Energia vs Distância
    ax4 = axes[1, 0]
    for tech, df in dados.items():
        if not df.empty:
            sns.boxplot(data=df, x='Distância (m)', y='Consumo Energia (J)', ax=ax4, color=cores[tech], alpha=0.7)
    ax4.set_title('Consumo de Energia vs Distância', fontweight='bold')
    ax4.set_xlabel('Distância (m)')
    ax4.set_ylabel('Consumo Energia (J)')
    
    # 5. Utilização do Canal vs Distância
    ax5 = axes[1, 1]
    for tech, df in dados.items():
        if not df.empty:
            sns.boxplot(data=df, x='Distância (m)', y='Utilização do Canal (%)', ax=ax5, color=cores[tech], alpha=0.7)
    ax5.set_title('Utilização do Canal vs Distância', fontweight='bold')
    ax5.set_xlabel('Distância (m)')
    ax5.set_ylabel('Utilização do Canal (%)')
    
    # 6. Comparação geral de PDR
    ax6 = axes[1, 2]
    pdr_data = []
    tech_labels = []
    
    for tech, df in dados.items():
        if not df.empty:
            pdr_data.extend(df['PDR (%)'].values)
            tech_labels.extend([tech.upper()] * len(df))
    
    if pdr_data:
        pdr_df = pd.DataFrame({'Tecnologia': tech_labels, 'PDR (%)': pdr_data})
        sns.boxplot(data=pdr_df, x='Tecnologia', y='PDR (%)', ax=ax6)
        ax6.set_title('PDR por Tecnologia', fontweight='bold')
        ax6.set_xlabel('Tecnologia')
        ax6.set_ylabel('PDR (%)')
    
    plt.tight_layout()
    plt.savefig('comparacao_tecnologias.png', dpi=300, bbox_inches='tight')
    plt.show()

def gerar_graficos_individual(dados):
    """Gera gráficos individuais para cada tecnologia"""
    
    for tech, df in dados.items():
        if df.empty:
            continue
            
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        fig.suptitle(f'Análise Detalhada - {tech.upper()}', fontsize=14, fontweight='bold')
        
        # 1. PDR vs Distância
        sns.boxplot(data=df, x='Distância (m)', y='PDR (%)', ax=axes[0, 0])
        axes[0, 0].set_title('PDR vs Distância')
        
        # 2. Vazão vs Distância
        sns.boxplot(data=df, x='Distância (m)', y='Vazão (bps)', ax=axes[0, 1])
        axes[0, 1].set_title('Vazão vs Distância')
        
        # 3. Atraso vs Distância
        sns.boxplot(data=df, x='Distância (m)', y='Atraso Médio (s)', ax=axes[1, 0])
        axes[1, 0].set_title('Atraso vs Distância')
        
        # 4. Consumo de Energia vs Distância
        sns.boxplot(data=df, x='Distância (m)', y='Consumo Energia (J)', ax=axes[1, 1])
        axes[1, 1].set_title('Consumo de Energia vs Distância')
        
        plt.tight_layout()
        plt.savefig(f'analise_{tech}.png', dpi=300, bbox_inches='tight')
        plt.show()

def gerar_relatorio_estatistico(dados):
    """Gera relatório estatístico detalhado"""
    
    with open('relatorio_experimentos.txt', 'w', encoding='utf-8') as f:
        f.write("RELATÓRIO DE EXPERIMENTOS LPWAN\n")
        f.write("=" * 50 + "\n\n")
        f.write(f"Data de geração: {pd.Timestamp.now().strftime('%Y-%m-%d %H:%M:%S')}\n\n")
        
        for tech in ['sigfox', 'lorawan', 'nbiot']:
            if tech in dados and not dados[tech].empty:
                df = dados[tech]
                f.write(f"\n{tech.upper()}:\n")
                f.write("-" * 20 + "\n")
                f.write(f"Total de execuções: {len(df)}\n")
                f.write(f"Distâncias testadas: {sorted(df['Distância (m)'].unique())}\n")
                f.write(f"Número de dispositivos testados: {sorted(df['Dispositivos'].unique())}\n\n")
                
                # Estatísticas por métrica
                metricas = ['PDR (%)', 'PLR (%)', 'Vazão (bps)', 'Atraso Médio (s)', 'Consumo Energia (J)']
                
                for metrica in metricas:
                    if metrica in df.columns:
                        f.write(f"{metrica}:\n")
                        f.write(f"  Média: {df[metrica].mean():.2f}\n")
                        f.write(f"  Mediana: {df[metrica].median():.2f}\n")
                        f.write(f"  Desvio Padrão: {df[metrica].std():.2f}\n")
                        f.write(f"  Mínimo: {df[metrica].min():.2f}\n")
                        f.write(f"  Máximo: {df[metrica].max():.2f}\n\n")
        
        # Comparação entre tecnologias
        f.write("\nCOMPARAÇÃO ENTRE TECNOLOGIAS:\n")
        f.write("-" * 30 + "\n")
        
        for metrica in ['PDR (%)', 'Vazão (bps)', 'Atraso Médio (s)', 'Consumo Energia (J)']:
            f.write(f"\n{metrica}:\n")
            for tech in ['sigfox', 'lorawan', 'nbiot']:
                if tech in dados and not dados[tech].empty and metrica in dados[tech].columns:
                    media = dados[tech][metrica].mean()
                    f.write(f"  {tech.upper()}: {media:.2f}\n")

def gerar_tabela_comparativa(dados):
    """Gera tabela comparativa em formato CSV"""
    
    comparacao = []
    
    for tech in ['sigfox', 'lorawan', 'nbiot']:
        if tech in dados and not dados[tech].empty:
            df = dados[tech]
            
            # Calcular médias por distância
            for distancia in sorted(df['Distância (m)'].unique()):
                subset = df[df['Distância (m)'] == distancia]
                
                row = {
                    'Tecnologia': tech.upper(),
                    'Distância (m)': distancia,
                    'PDR Médio (%)': subset['PDR (%)'].mean(),
                    'PLR Médio (%)': subset['PLR (%)'].mean(),
                    'Vazão Média (bps)': subset['Vazão (bps)'].mean(),
                    'Atraso Médio (s)': subset['Atraso Médio (s)'].mean(),
                    'Consumo Energia Médio (J)': subset['Consumo Energia (J)'].mean(),
                    'Utilização Canal Média (%)': subset['Utilização do Canal (%)'].mean()
                }
                comparacao.append(row)
    
    if comparacao:
        df_comparacao = pd.DataFrame(comparacao)
        df_comparacao.to_csv('tabela_comparativa.csv', index=False)
        print("Tabela comparativa gerada: tabela_comparativa.csv")
        
        # Exibir tabela formatada
        print("\nTABELA COMPARATIVA:")
        print(df_comparacao.to_string(index=False))

def main():
    print("=== ANÁLISE DE RESULTADOS LPWAN ===\n")
    
    # Carregar dados
    print("Carregando dados...")
    dados = carregar_dados()
    
    if not dados:
        print("Nenhum arquivo CSV encontrado!")
        print("Execute primeiro os experimentos e gere os arquivos CSV.")
        return
    
    print(f"Dados carregados para {len(dados)} tecnologias\n")
    
    # Gerar análises
    print("Gerando gráficos comparativos...")
    gerar_graficos_comparativos(dados)
    
    print("Gerando gráficos individuais...")
    gerar_graficos_individual(dados)
    
    print("Gerando relatório estatístico...")
    gerar_relatorio_estatistico(dados)
    
    print("Gerando tabela comparativa...")
    gerar_tabela_comparativa(dados)
    
    print("\n=== ANÁLISE CONCLUÍDA ===")
    print("Arquivos gerados:")
    print("- comparacao_tecnologias.png")
    print("- analise_sigfox.png, analise_lorawan.png, analise_nbiot.png")
    print("- relatorio_experimentos.txt")
    print("- tabela_comparativa.csv")

if __name__ == "__main__":
    main() 