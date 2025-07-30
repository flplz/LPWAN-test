# 🐧 Guia Específico para WSL - Experimentos LPWAN

## ✅ **WSL É PERFEITO PARA NS-3!**

O WSL (Windows Subsystem for Linux) é **excelente** para executar os experimentos LPWAN. Na verdade, é uma das melhores opções porque:

### 🎯 **Vantagens do WSL**
- ✅ **Ambiente Linux nativo** - NS-3 funciona perfeitamente
- ✅ **Performance próxima ao Linux** - Compilação rápida
- ✅ **Integração com Windows** - Acesso fácil aos arquivos
- ✅ **Estabilidade** - Menos problemas de compatibilidade
- ✅ **Suporte oficial** - Microsoft mantém atualizado

## 🚀 **Configuração Específica para WSL**

### **1. Verificar Versão do WSL**
```bash
# Verificar versão do WSL
wsl --version

# Verificar distribuição Linux
cat /etc/os-release
```

### **2. Atualizar Sistema**
```bash
# Atualizar pacotes
sudo apt update && sudo apt upgrade -y

# Instalar ferramentas essenciais
sudo apt install -y build-essential git curl wget
```

### **3. Configurar Memória (Opcional)**
Se você tiver problemas de memória durante a compilação:

```bash
# Criar arquivo de configuração do WSL
# No Windows, execute no PowerShell como administrador:
notepad "$env:USERPROFILE\.wslconfig"
```

Adicione estas linhas:
```
[wsl2]
memory=8GB
processors=4
swap=2GB
```

## 🔧 **Instalação no WSL**

### **Passo 1: Verificar Ambiente**
```bash
# Verificar se está no WSL
uname -a

# Verificar espaço em disco
df -h

# Verificar memória
free -h
```

### **Passo 2: Executar Script de Verificação**
```bash
cd ~/ns3_test/LPWAN-test
./verificar_instalacao.sh
```

### **Passo 3: Instalar NS-3 (se necessário)**
```bash
./instalar_ns3.sh
```

## ⚠️ **Considerações Específicas do WSL**

### **1. Performance de Compilação**
- **Primeira compilação**: Pode demorar 15-30 minutos
- **Compilações subsequentes**: Muito mais rápidas
- **Dica**: Use `make -j$(nproc)` para usar todos os cores

### **2. Espaço em Disco**
- NS-3 ocupa ~2-3 GB após compilação
- Verifique espaço disponível: `df -h`
- Limpe cache se necessário: `sudo apt clean`

### **3. Memória**
- Compilação pode usar 4-8 GB de RAM
- Se faltar memória, reduza jobs: `make -j2`

### **4. Acesso a Arquivos**
```bash
# Arquivos do Windows ficam em:
/mnt/c/Users/seu_usuario/

# Arquivos do WSL ficam em:
/home/seu_usuario/
```

## 🎯 **Execução dos Experimentos no WSL**

### **Comando Completo**
```bash
# 1. Navegar para o projeto
cd ~/ns3_test/LPWAN-test

# 2. Verificar instalação
./verificar_instalacao.sh

# 3. Executar experimentos
./run_experiments_simplificado.sh
```

### **Monitoramento de Recursos**
```bash
# Em outro terminal, monitorar recursos
htop

# Ou usar comandos simples
watch -n 1 'free -h && df -h'
```

## 🔍 **Solução de Problemas WSL**

### **Problema 1: Falta de Memória**
```bash
# Solução: Reduzir jobs de compilação
cd ~/ns3_test/ns-3-dev
make -j2  # Usar apenas 2 cores
```

### **Problema 2: Espaço em Disco**
```bash
# Verificar espaço
df -h

# Limpar cache
sudo apt clean
sudo apt autoremove

# Limpar arquivos temporários
rm -rf /tmp/*
```

### **Problema 3: Performance Lenta**
```bash
# Verificar se está usando WSL2
wsl --version

# Se estiver usando WSL1, atualizar para WSL2
wsl --set-version Ubuntu 2
```

### **Problema 4: Acesso a Arquivos**
```bash
# Copiar arquivos do Windows para WSL
cp /mnt/c/Users/seu_usuario/arquivo.txt ~/

# Copiar resultados do WSL para Windows
cp ~/ns3_test/LPWAN-test/resultados_simplificado/* /mnt/c/Users/seu_usuario/Desktop/
```

## 📊 **Otimizações para WSL**

### **1. Configurar Swap**
```bash
# Verificar swap atual
free -h

# Se não houver swap, criar
sudo fallocate -l 2G /swapfile
sudo chmod 600 /swapfile
sudo mkswap /swapfile
sudo swapon /swapfile
```

### **2. Otimizar Compilação**
```bash
# Usar todos os cores disponíveis
export MAKEFLAGS="-j$(nproc)"

# Ou especificar número de cores
export MAKEFLAGS="-j4"
```

### **3. Configurar Cache**
```bash
# Configurar ccache para compilações mais rápidas
sudo apt install ccache
export PATH="/usr/lib/ccache:$PATH"
```

## 🎯 **Fluxo de Trabalho Recomendado**

### **1. Preparação**
```bash
# Atualizar WSL
sudo apt update && sudo apt upgrade -y

# Verificar recursos
free -h && df -h
```

### **2. Instalação**
```bash
cd ~/ns3_test/LPWAN-test
./verificar_instalacao.sh
./instalar_ns3.sh
```

### **3. Execução**
```bash
./run_experiments_simplificado.sh
```

### **4. Análise**
```bash
python3 analise_resultados.py
```

### **5. Backup dos Resultados**
```bash
# Copiar resultados para Windows
cp -r resultados_simplificado/ /mnt/c/Users/seu_usuario/Desktop/LPWAN_Resultados/
```

## 🚀 **Comandos Rápidos para WSL**

```bash
# Verificar status do WSL
wsl --status

# Reiniciar WSL (se necessário)
wsl --shutdown
wsl

# Verificar recursos
htop

# Monitorar experimentos
tail -f resultados_simplificado/resultados_sigfox_1000m_10dev_1.txt
```

## ✅ **Conclusão**

**WSL é PERFEITO para seus experimentos LPWAN!**

### **Vantagens:**
- ✅ Ambiente Linux nativo
- ✅ Performance excelente
- ✅ Integração com Windows
- ✅ Estabilidade comprovada

### **Recomendação:**
**Continue usando WSL sem problemas!** É uma das melhores opções para desenvolvimento e pesquisa com NS-3.

### **Próximos Passos:**
```bash
# 1. Verificar seu ambiente
./verificar_instalacao.sh

# 2. Executar experimentos
./run_experiments_simplificado.sh

# 3. Analisar resultados
python3 analise_resultados.py
```

**Seu WSL está pronto para executar todos os experimentos LPWAN!** 🎉 