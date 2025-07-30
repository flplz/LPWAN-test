#!/bin/bash

echo "=== VERIFICAÇÃO DE INSTALAÇÃO NS-3 E MÓDULOS LPWAN ==="
echo ""

# Verificar se NS-3 está instalado
echo "1. Verificando NS-3..."
if [ -d "$HOME/ns3_test/ns-3-dev" ]; then
    echo "   ✅ NS-3 encontrado em ~/ns3_test/ns-3-dev"
    NS3_PATH="$HOME/ns3_test/ns-3-dev"
elif [ -d "/usr/local/ns-3-dev" ]; then
    echo "   ✅ NS-3 encontrado em /usr/local/ns-3-dev"
    NS3_PATH="/usr/local/ns-3-dev"
elif command -v ns3 &> /dev/null; then
    echo "   ✅ NS-3 encontrado no PATH"
    NS3_PATH=$(which ns3 | xargs dirname | xargs dirname)
else
    echo "   ❌ NS-3 não encontrado"
    NS3_PATH=""
fi

# Verificar módulos disponíveis
if [ ! -z "$NS3_PATH" ]; then
    echo ""
    echo "2. Verificando módulos disponíveis..."
    cd "$NS3_PATH"
    
    if [ -f "./ns3" ]; then
        echo "   Módulos disponíveis:"
        ./ns3 list 2>/dev/null | head -20
        
        echo ""
        echo "   Verificando módulos específicos:"
        
        # Verificar LTE
        if ./ns3 list 2>/dev/null | grep -q "lte"; then
            echo "   ✅ Módulo LTE disponível (para NB-IoT)"
        else
            echo "   ❌ Módulo LTE não encontrado"
        fi
        
        # Verificar Energy
        if ./ns3 list 2>/dev/null | grep -q "energy"; then
            echo "   ✅ Módulo Energy disponível"
        else
            echo "   ❌ Módulo Energy não encontrado"
        fi
        
        # Verificar LoRaWAN
        if ./ns3 list 2>/dev/null | grep -q "lorawan\|lora"; then
            echo "   ✅ Módulo LoRa/LoRaWAN disponível"
        else
            echo "   ❌ Módulo LoRa/LoRaWAN não encontrado"
        fi
        
        # Verificar Sigfox
        if ./ns3 list 2>/dev/null | grep -q "sigfox"; then
            echo "   ✅ Módulo Sigfox disponível"
        else
            echo "   ❌ Módulo Sigfox não encontrado"
        fi
    else
        echo "   ❌ Executável ns3 não encontrado"
    fi
fi

# Verificar dependências do sistema
echo ""
echo "3. Verificando dependências do sistema..."

# GCC
if command -v gcc &> /dev/null; then
    echo "   ✅ GCC instalado: $(gcc --version | head -1)"
else
    echo "   ❌ GCC não encontrado"
fi

# Python3
if command -v python3 &> /dev/null; then
    echo "   ✅ Python3 instalado: $(python3 --version)"
else
    echo "   ❌ Python3 não encontrado"
fi

# Git
if command -v git &> /dev/null; then
    echo "   ✅ Git instalado: $(git --version)"
else
    echo "   ❌ Git não encontrado"
fi

# CMake
if command -v cmake &> /dev/null; then
    echo "   ✅ CMake instalado: $(cmake --version | head -1)"
else
    echo "   ❌ CMake não encontrado"
fi

# Verificar bibliotecas Python
echo ""
echo "4. Verificando bibliotecas Python..."
python3 -c "import pandas" 2>/dev/null && echo "   ✅ pandas instalado" || echo "   ❌ pandas não encontrado"
python3 -c "import numpy" 2>/dev/null && echo "   ✅ numpy instalado" || echo "   ❌ numpy não encontrado"
python3 -c "import matplotlib" 2>/dev/null && echo "   ✅ matplotlib instalado" || echo "   ❌ matplotlib não encontrado"
python3 -c "import seaborn" 2>/dev/null && echo "   ✅ seaborn instalado" || echo "   ❌ seaborn não encontrado"

# Verificar bibliotecas do sistema
echo ""
echo "5. Verificando bibliotecas do sistema..."
ldconfig -p | grep -q "libsqlite3" && echo "   ✅ libsqlite3 encontrada" || echo "   ❌ libsqlite3 não encontrada"
ldconfig -p | grep -q "libboost" && echo "   ✅ libboost encontrada" || echo "   ❌ libboost não encontrada"
ldconfig -p | grep -q "libxml2" && echo "   ✅ libxml2 encontrada" || echo "   ❌ libxml2 não encontrada"

echo ""
echo "=== RECOMENDAÇÕES ==="

if [ -z "$NS3_PATH" ]; then
    echo "❌ NS-3 não está instalado."
    echo "   Execute:"
    echo "   cd ~/ns3_test"
    echo "   git clone https://gitlab.com/nsnam/ns-3-dev.git"
    echo "   cd ns-3-dev"
    echo "   ./ns3 configure --enable-examples --enable-tests --enable-modules=lte,energy"
    echo "   ./ns3 build"
elif [ ! -f "$NS3_PATH/ns3" ]; then
    echo "❌ NS-3 não está compilado."
    echo "   Execute:"
    echo "   cd $NS3_PATH"
    echo "   ./ns3 configure --enable-examples --enable-tests --enable-modules=lte,energy"
    echo "   ./ns3 build"
else
    echo "✅ NS-3 está instalado e compilado."
    echo "   Você pode prosseguir com os experimentos."
fi

echo ""
echo "=== PRÓXIMOS PASSOS ==="
echo "1. Se NS-3 não estiver instalado, siga o guia de instalação"
echo "2. Se estiver instalado, teste com:"
echo "   cd ~/ns3_test/LPWAN-test"
echo "   chmod +x run_experiments.sh"
echo "   ./run_experiments.sh"
echo ""
echo "Para mais detalhes, consulte: GUIA_INSTALACAO_MODULOS_LPWAN.md" 