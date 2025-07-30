#!/usr/bin/env python3
"""
Script para converter saídas dos experimentos LPWAN para CSV padronizado
Autor: Assistente IA
Data: 2024
"""

import pandas as pd
import re
import sys
import os
import glob

def parse_sigfox_output(output_file):
    """Converte saída do Sigfox para CSV"""
    data = {}
    
    try:
        with open(output_file, 'r') as f:
            content = f.read()
        
        # Extrair valores usando regex
        patterns = {
            'execucao': r'Execução:\s*(\d+)',
            'distancia': r'Distância \(m\):\s*([\d.]+)',
            'dispositivos': r'Dispositivos:\s*(\d+)',
            'enviados': r'Pacotes Enviados:\s*(\d+)',
            'recebidos': r'Pacotes Recebidos:\s*(\d+)',
            'duplicados': r'Pacotes Duplicados:\s*(\d+)',
            'pdr': r'PDR \(%\):\s*([\d.]+)',
            'plr': r'PLR \(%\):\s*([\d.]+)',
            'retransmissao': r'Retransmissão \(%\):\s*([\d.]+)',
            'vazao': r'Vazão \(bps\):\s*([\d.]+)',
            'utilizacao': r'Utilização do Canal \(%\):\s*([\d.]+)',
            'atraso': r'Atraso Médio \(s\):\s*([\d.]+)',
            'banda': r'Banda Utilizada \(kHz\):\s*([\d.]+)',
            'energia': r'Consumo Energia \(J\):\s*([\d.]+)'
        }
        
        for key, pattern in patterns.items():
            match = re.search(pattern, content)
            if match:
                data[key] = match.group(1)
            else:
                data[key] = 0
        
        return pd.DataFrame([{
            'Execução': int(data['execucao']),
            'Distância (m)': float(data['distancia']),
            'Dispositivos': int(data['dispositivos']),
            'Pacotes Enviados': int(data['enviados']),
            'Pacotes Recebidos': int(data['recebidos']),
            'Pacotes Duplicados': int(data['duplicados']),
            'PDR (%)': float(data['pdr']),
            'PLR (%)': float(data['plr']),
            'Retransmissão (%)': float(data['retransmissao']),
            'Vazão (bps)': float(data['vazao']),
            'Utilização do Canal (%)': float(data['utilizacao']),
            'Atraso Médio (s)': float(data['atraso']),
            'Banda Utilizada (kHz)': float(data['banda']),
            'Consumo Energia (J)': float(data['energia'])
        }])
        
    except Exception as e:
        print(f"Erro ao processar arquivo Sigfox {output_file}: {e}")
        return pd.DataFrame()

def parse_lorawan_output(output_file):
    """Converte saída do LoRaWAN para CSV"""
    data = {}
    
    try:
        with open(output_file, 'r') as f:
            content = f.read()
        
        # Extrair valores usando regex
        patterns = {
            'execucao': r'Execução:\s*(\d+)',
            'distancia': r'Distância \(m\):\s*([\d.]+)',
            'dispositivos': r'Dispositivos:\s*(\d+)',
            'enviados': r'Pacotes Enviados:\s*(\d+)',
            'recebidos': r'Pacotes Recebidos:\s*(\d+)',
            'duplicados': r'Pacotes Duplicados:\s*(\d+)',
            'pdr': r'PDR \(%\):\s*([\d.]+)',
            'plr': r'PLR \(%\):\s*([\d.]+)',
            'retransmissao': r'Retransmissão \(%\):\s*([\d.]+)',
            'vazao': r'Vazão \(bps\):\s*([\d.]+)',
            'utilizacao': r'Utilização do Canal \(%\):\s*([\d.]+)',
            'atraso': r'Atraso Médio \(s\):\s*([\d.]+)',
            'banda': r'Banda Utilizada \(kHz\):\s*([\d.]+)',
            'energia': r'Consumo Energia \(J\):\s*([\d.]+)'
        }
        
        for key, pattern in patterns.items():
            match = re.search(pattern, content)
            if match:
                data[key] = match.group(1)
            else:
                data[key] = 0
        
        return pd.DataFrame([{
            'Execução': int(data['execucao']),
            'Distância (m)': float(data['distancia']),
            'Dispositivos': int(data['dispositivos']),
            'Pacotes Enviados': int(data['enviados']),
            'Pacotes Recebidos': int(data['recebidos']),
            'Pacotes Duplicados': int(data['duplicados']),
            'PDR (%)': float(data['pdr']),
            'PLR (%)': float(data['plr']),
            'Retransmissão (%)': float(data['retransmissao']),
            'Vazão (bps)': float(data['vazao']),
            'Utilização do Canal (%)': float(data['utilizacao']),
            'Atraso Médio (s)': float(data['atraso']),
            'Banda Utilizada (kHz)': float(data['banda']),
            'Consumo Energia (J)': float(data['energia'])
        }])
        
    except Exception as e:
        print(f"Erro ao processar arquivo LoRaWAN {output_file}: {e}")
        return pd.DataFrame()

def parse_nbiot_output(output_file):
    """Converte saída do NB-IoT para CSV"""
    data = {}
    
    try:
        with open(output_file, 'r') as f:
            content = f.read()
        
        # Extrair valores usando regex
        patterns = {
            'execucao': r'Execução:\s*(\d+)',
            'distancia': r'Distância \(m\):\s*([\d.]+)',
            'dispositivos': r'Dispositivos:\s*(\d+)',
            'enviados': r'Pacotes Enviados:\s*(\d+)',
            'recebidos': r'Pacotes Recebidos:\s*(\d+)',
            'duplicados': r'Pacotes Duplicados:\s*(\d+)',
            'pdr': r'PDR \(%\):\s*([\d.]+)',
            'plr': r'PLR \(%\):\s*([\d.]+)',
            'retransmissao': r'Retransmissão \(%\):\s*([\d.]+)',
            'vazao': r'Vazão \(bps\):\s*([\d.]+)',
            'utilizacao': r'Utilização do Canal \(%\):\s*([\d.]+)',
            'atraso': r'Atraso Médio \(s\):\s*([\d.]+)',
            'banda': r'Banda Utilizada \(kHz\):\s*([\d.]+)',
            'energia': r'Consumo Energia \(J\):\s*([\d.]+)'
        }
        
        for key, pattern in patterns.items():
            match = re.search(pattern, content)
            if match:
                data[key] = match.group(1)
            else:
                data[key] = 0
        
        return pd.DataFrame([{
            'Execução': int(data['execucao']),
            'Distância (m)': float(data['distancia']),
            'Dispositivos': int(data['dispositivos']),
            'Pacotes Enviados': int(data['enviados']),
            'Pacotes Recebidos': int(data['recebidos']),
            'Pacotes Duplicados': int(data['duplicados']),
            'PDR (%)': float(data['pdr']),
            'PLR (%)': float(data['plr']),
            'Retransmissão (%)': float(data['retransmissao']),
            'Vazão (bps)': float(data['vazao']),
            'Utilização do Canal (%)': float(data['utilizacao']),
            'Atraso Médio (s)': float(data['atraso']),
            'Banda Utilizada (kHz)': float(data['banda']),
            'Consumo Energia (J)': float(data['energia'])
        }])
        
    except Exception as e:
        print(f"Erro ao processar arquivo NB-IoT {output_file}: {e}")
        return pd.DataFrame()

def process_all_files():
    """Processa todos os arquivos de resultado encontrados"""
    tecnologias = ['sigfox', 'lorawan', 'nbiot']
    
    for tech in tecnologias:
        # Procurar por arquivos de resultado
        pattern = f"resultados_{tech}*.txt"
        files = glob.glob(pattern)
        
        if not files:
            print(f"Nenhum arquivo encontrado para {tech}")
            continue
        
        all_data = []
        for file in files:
            print(f"Processando {file}...")
            
            if tech == 'sigfox':
                df = parse_sigfox_output(file)
            elif tech == 'lorawan':
                df = parse_lorawan_output(file)
            elif tech == 'nbiot':
                df = parse_nbiot_output(file)
            
            if not df.empty:
                all_data.append(df)
        
        if all_data:
            # Combinar todos os dados
            combined_df = pd.concat(all_data, ignore_index=True)
            
            # Salvar CSV
            output_csv = f"resultados_combinados_{tech}.csv"
            combined_df.to_csv(output_csv, index=False)
            print(f"CSV gerado: {output_csv}")
            print(f"Total de registros: {len(combined_df)}")

def main():
    if len(sys.argv) < 2:
        print("Uso:")
        print("  python converter_resultados.py <tecnologia> <arquivo_saida>")
        print("  python converter_resultados.py --all")
        sys.exit(1)
    
    if sys.argv[1] == "--all":
        process_all_files()
    else:
        if len(sys.argv) < 3:
            print("Erro: arquivo de saída não especificado")
            sys.exit(1)
        
        tecnologia = sys.argv[1]
        arquivo_saida = sys.argv[2]
        
        if tecnologia == 'sigfox':
            df = parse_sigfox_output(arquivo_saida)
        elif tecnologia == 'lorawan':
            df = parse_lorawan_output(arquivo_saida)
        elif tecnologia == 'nbiot':
            df = parse_nbiot_output(arquivo_saida)
        else:
            print("Tecnologia não reconhecida. Use: sigfox, lorawan, ou nbiot")
            sys.exit(1)
        
        if not df.empty:
            # Salvar CSV
            output_csv = arquivo_saida.replace('.txt', '.csv')
            df.to_csv(output_csv, index=False)
            print(f"CSV gerado: {output_csv}")
        else:
            print("Erro: não foi possível processar o arquivo")

if __name__ == "__main__":
    main() 