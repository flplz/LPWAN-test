
# Explicação da Simulação Sigfox com NS-3

## Objetivo
Simular um cenário Sigfox com um dispositivo e um gateway, analisando:
- Consumo energético do dispositivo.
- Entrega e perda de pacotes transmitidos.
- Simulação de medições periódicas e descarga da bateria.

## Etapas da Simulação

1. **Inicialização de parâmetros**
   - Define tempo de simulação e número de dispositivos.
   - Define capacidade inicial de bateria.

2. **Criação do canal e propagação**
   - Usa modelo log-distance para perda de propagação.
   - Define atraso de propagação constante.

3. **Configuração de mobilidade**
   - Dispositivos fixos em uma mesma posição (modelo constante).

4. **Instalação dos dispositivos Sigfox**
   - Um end device (EP) e um gateway (GW) são configurados com os módulos Sigfox.

5. **Modelagem de energia**
   - Uso do `SdcEnergySource` com energia inicial de 3600J.
   - Medição de consumo pelo rádio via `SigfoxRadioEnergyModel`.

6. **Aplicação periódica**
   - Envia pacotes de 12 bytes a cada 60 segundos.

7. **Coleta de resultados**
   - Mede energia restante e consumo.
   - Analisa pacotes recebidos e perdidos usando rastreadores.
   - Exporta dados para arquivos `.txt`.

8. **Execução da simulação**
   - Agenda tarefas (medições, descarga, impressão).
   - Executa e destrói o simulador.

## Resultados Gerados
- `BatteryLevel.txt`: nível de energia ao longo do tempo.
- `CurrentGraph.txt`: consumo de corrente do sistema.
- Saída do terminal: quantidade de pacotes entregues vs perdidos.

---
Ideal para simulações de LPWAN (como Sigfox) considerando fatores de energia e cobertura.
