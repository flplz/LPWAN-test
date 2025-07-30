#!/bin/bash

echo "=== INSTALAÇÃO AUTOMATIZADA DO NS-3 PARA EXPERIMENTOS LPWAN ==="
echo ""

# Verificar se estamos no diretório correto
if [ ! -d "$HOME/ns3_test" ]; then
    echo "Criando diretório ~/ns3_test..."
    mkdir -p ~/ns3_test
fi

cd ~/ns3_test

# Verificar se NS-3 já está instalado
if [ -d "ns-3-dev" ]; then
    echo "NS-3 já está instalado em ~/ns3_test/ns-3-dev"
    echo "Deseja reinstalar? (s/n)"
    read -r response
    if [[ "$response" =~ ^[Ss]$ ]]; then
        echo "Removendo instalação anterior..."
        rm -rf ns-3-dev
    else
        echo "Usando instalação existente..."
        cd ns-3-dev
        if [ -f "./ns3" ]; then
            echo "NS-3 já está compilado!"
            echo "Testando instalação..."
            ./ns3 --version
            echo ""
            echo "Módulos disponíveis:"
            ./ns3 list | head -10
            echo ""
            echo "✅ NS-3 está pronto para uso!"
            echo "Volte para o diretório do projeto:"
            echo "cd ~/ns3_test/LPWAN-test"
            exit 0
        fi
    fi
fi

echo "Instalando dependências do sistema..."

# Detectar distribuição Linux
if [ -f /etc/debian_version ]; then
    echo "Detectado sistema Debian/Ubuntu"
    sudo apt-get update
    sudo apt-get install -y build-essential git python3 python3-pip cmake \
        libsqlite3-dev libboost-all-dev libxml2-dev libgtk-3-dev \
        libgsl-dev libgslcblas0 qt5-default
elif [ -f /etc/redhat-release ]; then
    echo "Detectado sistema RedHat/CentOS"
    sudo yum groupinstall -y "Development Tools"
    sudo yum install -y git python3 python3-pip cmake \
        sqlite-devel boost-devel libxml2-devel gtk3-devel \
        gsl-devel qt5-qtbase-devel
else
    echo "Sistema não reconhecido. Instalando dependências básicas..."
    sudo apt-get update
    sudo apt-get install -y build-essential git python3 python3-pip cmake
fi

echo "Instalando bibliotecas Python..."
pip3 install pandas numpy matplotlib seaborn

echo ""
echo "Baixando NS-3..."
git clone https://gitlab.com/nsnam/ns-3-dev.git
cd ns-3-dev

echo ""
echo "Configurando NS-3 com módulos necessários..."

# Tentar configurar com módulos específicos
if ./ns3 configure --enable-examples --enable-tests --enable-modules=lte,energy,network,mobility,wifi,applications,propagation; then
    echo "✅ Configuração com módulos específicos bem-sucedida"
else
    echo "⚠️  Configuração com módulos específicos falhou, tentando configuração básica..."
    if ./ns3 configure --enable-examples --enable-tests; then
        echo "✅ Configuração básica bem-sucedida"
    else
        echo "❌ Falha na configuração do NS-3"
        echo "Verifique as dependências e tente novamente"
        exit 1
    fi
fi

echo ""
echo "Compilando NS-3 (isso pode demorar alguns minutos)..."
if ./ns3 build; then
    echo "✅ Compilação bem-sucedida!"
else
    echo "❌ Falha na compilação"
    echo "Verifique os erros acima e tente novamente"
    exit 1
fi

echo ""
echo "Testando instalação..."
./ns3 --version

echo ""
echo "Módulos disponíveis:"
./ns3 list | head -10

echo ""
echo "Testando exemplo básico..."
if ./ns3 run "examples/wireless/wifi-simple-adhoc-grid" --quiet; then
    echo "✅ Teste básico bem-sucedido!"
else
    echo "⚠️  Teste básico falhou, mas NS-3 está instalado"
fi

echo ""
echo "=== INSTALAÇÃO CONCLUÍDA ==="
echo "✅ NS-3 instalado com sucesso em ~/ns3_test/ns-3-dev"
echo ""
echo "Próximos passos:"
echo "1. Volte para o diretório do projeto:"
echo "   cd ~/ns3_test/LPWAN-test"
echo ""
echo "2. Execute o script de verificação:"
echo "   chmod +x verificar_instalacao.sh"
echo "   ./verificar_instalacao.sh"
echo ""
echo "3. Execute os experimentos:"
echo "   chmod +x run_experiments.sh"
echo "   ./run_experiments.sh"
echo ""
echo "Para mais informações, consulte:"
echo "- GUIA_INSTALACAO_MODULOS_LPWAN.md"
echo "- README.md" 