#!/usr/bin/env python3
"""
Análise Simples dos Resultados LPWAN
"""

import pandas as pd
import matplotlib.pyplot as plt

def main():
    print("🚀 ANÁLISE SIMPLES DOS RESULTADOS LPWAN")
    print("="*50)
    
    # Carregar dados
    try:
        df = pd.read_csv('resultados/resultados_extendidos_completo.csv')
        print(f"✅ Dados carregados: {len(df)} registros")
        
        # Estatísticas básicas
        print(f"\n📊 Total de experimentos: {len(df)}")
        print(f"📡 Tecnologias: {df['Tecnologia'].unique()}")
        print(f"📍 Distâncias: {sorted(df['Distância (m)'].unique())}")
        print(f"📱 Dispositivos: {sorted(df['Dispositivos'].unique())}")
        
        # Comparação por tecnologia
        print("\n🔬 COMPARAÇÃO POR TECNOLOGIA:")
        for tech in df['Tecnologia'].unique():
            tech_data = df[df['Tecnologia'] == tech]
            print(f"\n📡 {tech}:")
            print(f"   • PDR médio: {tech_data['PDR (%)'].mean():.1f}%")
            print(f"   • Vazão média: {tech_data['Vazão (bps)'].mean():.0f} bps")
            print(f"   • Consumo energético: {tech_data['Consumo Energia (J)'].mean():.1f} J")
            print(f"   • Atraso médio: {tech_data['Atraso Médio (s)'].mean():.3f} s")
        
        # Melhores tecnologias
        print("\n🏆 MELHORES TECNOLOGIAS:")
        pdr_medio = df.groupby('Tecnologia')['PDR (%)'].mean()
        melhor_pdr = pdr_medio.idxmax()
        print(f"   • Melhor PDR: {melhor_pdr} ({pdr_medio[melhor_pdr]:.1f}%)")
        
        energia_medio = df.groupby('Tecnologia')['Consumo Energia (J)'].mean()
        melhor_energia = energia_medio.idxmin()
        print(f"   • Mais eficiente: {melhor_energia} ({energia_medio[melhor_energia]:.1f} J)")
        
        vazao_medio = df.groupby('Tecnologia')['Vazão (bps)'].mean()
        melhor_vazao = vazao_medio.idxmax()
        print(f"   • Maior vazão: {melhor_vazao} ({vazao_medio[melhor_vazao]:.0f} bps)")
        
        # Impacto da distância
        print("\n📊 IMPACTO DA DISTÂNCIA:")
        for distancia in [3000, 10000, 30000, 50000]:
            dist_data = df[df['Distância (m)'] == distancia]
            pdr_dist = dist_data.groupby('Tecnologia')['PDR (%)'].mean()
            print(f"   • {distancia}m: {dict(pdr_dist)}")
        
        print("\n✅ Análise concluída!")
        
    except Exception as e:
        print(f"❌ Erro: {e}")

if __name__ == "__main__":
    main() 