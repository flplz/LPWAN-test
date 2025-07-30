#!/bin/bash

# Script para executar experimentos LPWAN automaticamente
# Autor: Assistente IA
# Data: 2024

echo "=== EXECUÇÃO AUTOMATIZADA DE EXPERIMENTOS LPWAN ==="
echo ""

# Configurações dos experimentos
TECNOLOGIAS=("sigfox" "lorawan" "nbiot")
DISTANCIAS=(1000 2000 3000 4000 5000)
DISPOSITIVOS=(10 50 100)
EXECUCOES=5

# Criar diretório para resultados
mkdir -p resultados
cd resultados

echo "Configurações:"
echo "- Tecnologias: ${TECNOLOGIAS[*]}"
echo "- Distâncias: ${DISTANCIAS[*]} metros"
echo "- Dispositivos: ${DISPOSITIVOS[*]}"
echo "- Execuções por cenário: $EXECUCOES"
echo ""

# Função para executar experimento Sigfox
executar_sigfox() {
    local distancia=$1
    local dispositivos=$2
    local execucao=$3
    
    echo "  Executando Sigfox - Distância: ${distancia}m, Dispositivos: $dispositivos, Execução: $execucao"
    
    # Compilar se necessário
    if [ ! -f "../sigfox/sigfox_sim" ]; then
        echo "    Compilando Sigfox..."
        cd ../sigfox
        g++ -o sigfox_sim sigfox_simulacao_explicada.cc -I/usr/include/ns3 -lns3-core -lns3-network -lns3-mobility -lns3-sigfox -lns3-energy 2>/dev/null
        cd ../resultados
    fi
    
    # Executar simulação
    ../sigfox/sigfox_sim --distance=$distancia --nDevices=$dispositivos > "resultados_sigfox_${distancia}m_${dispositivos}dev_${execucao}.txt" 2>&1
    
    # Verificar se gerou CSV
    if [ -f "../sigfox/resultados_sigfox.csv" ]; then
        cp ../sigfox/resultados_sigfox.csv "resultados_sigfox_${distancia}m_${dispositivos}dev_${execucao}.csv"
    fi
}

# Função para executar experimento LoRaWAN
executar_lorawan() {
    local distancia=$1
    local dispositivos=$2
    local execucao=$3
    
    echo "  Executando LoRaWAN - Distância: ${distancia}m, Dispositivos: $dispositivos, Execução: $execucao"
    
    # Compilar se necessário
    if [ ! -f "../lorawan/lorawan_sim" ]; then
        echo "    Compilando LoRaWAN..."
        cd ../lorawan
        g++ -o lorawan_sim lorawan-test.cc -I/usr/include/ns3 -lns3-core -lns3-network -lns3-mobility -lns3-lora -lns3-energy 2>/dev/null
        cd ../resultados
    fi
    
    # Executar simulação
    ../lorawan/lorawan_sim --distance=$distancia --deviceCount=$dispositivos --numExecutions=1 > "resultados_lorawan_${distancia}m_${dispositivos}dev_${execucao}.txt" 2>&1
    
    # Verificar se gerou CSV
    if [ -f "../lorawan/resultados_lorawan.csv" ]; then
        cp ../lorawan/resultados_lorawan.csv "resultados_lorawan_${distancia}m_${dispositivos}dev_${execucao}.csv"
    fi
}

# Função para executar experimento NB-IoT
executar_nbiot() {
    local distancia=$1
    local dispositivos=$2
    local execucao=$3
    
    echo "  Executando NB-IoT - Distância: ${distancia}m, Dispositivos: $dispositivos, Execução: $execucao"
    
    # Compilar se necessário
    if [ ! -f "../nbiot/nbiot_sim" ]; then
        echo "    Compilando NB-IoT..."
        cd ../nbiot
        g++ -o nbiot_sim nb-iot-sim.cc -I/usr/include/ns3 -lns3-core -lns3-network -lns3-mobility -lns3-lte -lns3-energy 2>/dev/null
        cd ../resultados
    fi
    
    # Executar simulação
    ../nbiot/nbiot_sim --distance=$distancia --nUe=$dispositivos > "resultados_nbiot_${distancia}m_${dispositivos}dev_${execucao}.txt" 2>&1
    
    # Verificar se gerou CSV
    if [ -f "../nbiot/resultados_nbiot.csv" ]; then
        cp ../nbiot/resultados_nbiot.csv "resultados_nbiot_${distancia}m_${dispositivos}dev_${execucao}.csv"
    fi
}

# Executar todos os experimentos
for tech in "${TECNOLOGIAS[@]}"; do
    echo "Executando experimentos para $tech..."
    
    for dist in "${DISTANCIAS[@]}"; do
        for dev in "${DISPOSITIVOS[@]}"; do
            echo "  Cenário: ${dist}m, $dev dispositivos"
            
            for exec in $(seq 1 $EXECUCOES); do
                case $tech in
                    "sigfox")
                        executar_sigfox $dist $dev $exec
                        ;;
                    "lorawan")
                        executar_lorawan $dist $dev $exec
                        ;;
                    "nbiot")
                        executar_nbiot $dist $dev $exec
                        ;;
                esac
                
                # Pequena pausa entre execuções
                sleep 1
            done
        done
    done
    echo ""
done

echo "=== CONVERSÃO PARA CSV ==="
echo "Convertendo resultados para CSV..."

# Mover para diretório pai para executar script de conversão
cd ..

# Executar script de conversão
if [ -f "converter_resultados.py" ]; then
    python3 converter_resultados.py --all
else
    echo "Script de conversão não encontrado!"
fi

echo "=== ANÁLISE DOS RESULTADOS ==="
echo "Gerando gráficos e relatórios..."

# Executar script de análise
if [ -f "analise_resultados.py" ]; then
    python3 analise_resultados.py
else
    echo "Script de análise não encontrado!"
fi

echo ""
echo "=== EXPERIMENTOS CONCLUÍDOS ==="
echo "Resultados salvos em: resultados/"
echo "CSVs gerados em: resultados/"
echo "Gráficos e relatórios gerados no diretório atual"
echo ""
echo "Arquivos gerados:"
echo "- comparacao_tecnologias.png"
echo "- analise_sigfox.png, analise_lorawan.png, analise_nbiot.png"
echo "- relatorio_experimentos.txt"
echo "- tabela_comparativa.csv"
echo ""
echo "Para reproduzir os experimentos, execute:"
echo "  chmod +x run_experiments.sh"
echo "  ./run_experiments.sh" 