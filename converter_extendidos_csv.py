#!/usr/bin/env python3
"""
Conversor de Resultados Extendidos para CSV
Converte todos os arquivos .txt em resultados_extendidos/ para CSV
"""

import os
import re
import pandas as pd
import glob

def parse_resultado_extendido(file_path):
    """Extrai métricas de um arquivo de resultado"""
    with open(file_path, 'r') as f:
        content = f.read()
    
    # Extrair métricas usando regex
    patterns = {
        'tecnologia': r'=== RESULTADOS (\w+) ===',
        'execucao': r'Execução: (\d+)',
        'distancia': r'Distância \(m\): (\d+)',
        'dispositivos': r'Dispositivos: (\d+)',
        'pacotes_enviados': r'Pacotes Enviados: (\d+)',
        'pacotes_recebidos': r'Pacotes Recebidos: (\d+)',
        'pacotes_duplicados': r'Pacotes Duplicados: (\d+)',
        'pdr': r'PDR \(%\): (\d+)',
        'plr': r'PLR \(%\): (\d+)',
        'retransmissao': r'Retransmissão \(%\): (\d+)',
        'vazao': r'Vazão \(bps\): (\d+)',
        'utilizacao': r'Utilização do Canal \(%\): (\d+)',
        'atraso': r'Atraso Médio \(s\): (\d+)',
        'banda': r'Banda Utilizada \(kHz\): (\d+)',
        'energia': r'Consumo Energia \(J\): (\d+)'
    }
    
    resultado = {}
    for key, pattern in patterns.items():
        match = re.search(pattern, content)
        if match:
            resultado[key] = match.group(1)
        else:
            resultado[key] = '0'
    
    return resultado

def converter_extendidos_csv():
    """Converte todos os arquivos .txt para CSV"""
    print("=== CONVERSOR DE RESULTADOS EXTENDIDOS PARA CSV ===")
    print("")
    
    # Verificar se o diretório existe
    if not os.path.exists('resultados_extendidos'):
        print("❌ Diretório 'resultados_extendidos' não encontrado!")
        print("Execute primeiro: ./gerar_experimentos_extendidos_simples.sh")
        return
    
    # Encontrar todos os arquivos .txt
    arquivos = glob.glob('resultados_extendidos/*.txt')
    
    if not arquivos:
        print("❌ Nenhum arquivo .txt encontrado em 'resultados_extendidos'")
        return
    
    print(f"📁 Encontrados {len(arquivos)} arquivos para converter")
    print("🔄 Convertendo...")
    
    # Lista para armazenar todos os resultados
    todos_resultados = []
    
    # Processar cada arquivo
    for i, arquivo in enumerate(arquivos):
        try:
            resultado = parse_resultado_extendido(arquivo)
            todos_resultados.append(resultado)
            
            # Mostrar progresso
            if (i + 1) % 100 == 0:
                print(f"   Progresso: {i + 1}/{len(arquivos)} arquivos")
                
        except Exception as e:
            print(f"⚠️  Erro ao processar {arquivo}: {e}")
    
    # Criar DataFrame
    df = pd.DataFrame(todos_resultados)
    
    # Converter colunas numéricas
    colunas_numericas = ['execucao', 'distancia', 'dispositivos', 'pacotes_enviados', 
                        'pacotes_recebidos', 'pacotes_duplicados', 'pdr', 'plr', 
                        'retransmissao', 'vazao', 'utilizacao', 'atraso', 'banda', 'energia']
    
    for col in colunas_numericas:
        if col in df.columns:
            df[col] = pd.to_numeric(df[col], errors='coerce')
    
    # Renomear colunas para português
    df = df.rename(columns={
        'tecnologia': 'Tecnologia',
        'execucao': 'Execução',
        'distancia': 'Distância (m)',
        'dispositivos': 'Dispositivos',
        'pacotes_enviados': 'Pacotes Enviados',
        'pacotes_recebidos': 'Pacotes Recebidos',
        'pacotes_duplicados': 'Pacotes Duplicados',
        'pdr': 'PDR (%)',
        'plr': 'PLR (%)',
        'retransmissao': 'Retransmissão (%)',
        'vazao': 'Vazão (bps)',
        'utilizacao': 'Utilização do Canal (%)',
        'atraso': 'Atraso Médio (s)',
        'banda': 'Banda Utilizada (kHz)',
        'energia': 'Consumo Energia (J)'
    })
    
    # Salvar CSV principal
    csv_principal = 'resultados_extendidos_completo.csv'
    df.to_csv(csv_principal, index=False)
    print(f"✅ CSV principal salvo: {csv_principal}")
    
    # Salvar CSVs separados por tecnologia
    for tecnologia in df['Tecnologia'].unique():
        df_tech = df[df['Tecnologia'] == tecnologia]
        csv_tech = f'resultados_extendidos_{tecnologia.lower()}.csv'
        df_tech.to_csv(csv_tech, index=False)
        print(f"✅ CSV {tecnologia}: {csv_tech} ({len(df_tech)} registros)")
    
    # Estatísticas
    print("")
    print("📊 ESTATÍSTICAS:")
    print(f"Total de registros: {len(df)}")
    print(f"Tecnologias: {df['Tecnologia'].nunique()}")
    print(f"Distâncias únicas: {df['Distância (m)'].nunique()}")
    print(f"Dispositivos únicos: {df['Dispositivos'].nunique()}")
    print(f"Execuções por configuração: {df.groupby(['Tecnologia', 'Distância (m)', 'Dispositivos']).size().min()}")
    
    # Mostrar primeiras linhas
    print("")
    print("📝 PRIMEIRAS LINHAS DO CSV:")
    print(df.head().to_string(index=False))
    
    print("")
    print("🎯 Arquivos CSV gerados:")
    print(f"   - {csv_principal} (todos os dados)")
    for tecnologia in df['Tecnologia'].unique():
        print(f"   - resultados_extendidos_{tecnologia.lower()}.csv")
    
    print("")
    print("=== CONVERSÃO CONCLUÍDA ===")

if __name__ == "__main__":
    converter_extendidos_csv() 