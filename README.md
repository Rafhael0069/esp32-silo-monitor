# Sistema de Monitoramento de Silo com ESP32

![ESP32](https://img.shields.io/badge/ESP32-Node--32-blue) ![C++](https://img.shields.io/badge/C++-Arduino-green) ![License](https://img.shields.io/badge/License-MIT-yellow) ![Status](https://img.shields.io/badge/Status-Em%20Desenvolvimento-orange)

Sistema embarcado para monitoramento automático de nível em silos utilizando sensor de distância VL53L0X e comunicação Wi-Fi com envio de dados para API REST.

## 📋 Índice

- [Visão Geral](#visão-geral)
- [Funcionalidades](#funcionalidades)
- [Hardware Requerido](#hardware-requerido)
- [Esquema de Ligação](#esquema-de-ligação)
- [Instalação e Configuração](#instalação-e-configuração)
- [API e Endpoints](#api-e-endpoints)
- [Estrutura do Projeto](#estrutura-do-projeto)
- [Configuração do Ambiente](#configuração-do-ambiente)
- [Uso e Operação](#uso-e-operação)
- [Monitoramento e Debug](#monitoramento-e-debug)
- [Solução de Problemas](#solução-de-problemas)
- [Licença](#licença)
- [Contato](#contato)

## 🎯 Visão Geral

Este projeto implementa um sistema IoT para monitoramento contínuo do nível de grãos em silos agrícolas. O sistema utiliza microcontrolador ESP32 com sensor de distância VL53L0X para medir a distância até a superfície do material armazenado e envia os dados via Wi-Fi para uma API REST em intervalos regulares.

### Diagrama de Blocos
```
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│   Sensor    │    │    ESP32    │    │    API      │
│  VL53L0X    │◄──►│   (Wi-Fi)   │◄──►│   REST      │
│             │    │             │    │  Render     │
└─────────────┘    └─────────────┘    └─────────────┘
```

## ✨ Funcionalidades

- **📏 Medição Precisas**: Sensor VL53L0X com precisão milimétrica
- **📡 Comunicação Wi-Fi**: Conexão automática e reconexão em caso de falha
- **🔒 Comunicação Segura**: Envio de dados via HTTPS com certificado SSL
- **💡 Indicadores Visuais**: LEDs para status do sistema
- **🔄 Monitoramento Contínuo**: Leituras automáticas em intervalos configuráveis
- **❌ Tratamento de Erros**: Recuperação automática de falhas
- **📊 Logs Detalhados**: Monitoramento serial para debug

## 🛠 Hardware Requerido

### Componentes Principais
| Componente | Quantidade | Observações |
|------------|------------|-------------|
| ESP32 DOIT DevKit v1 | 1 | Ou similar com Wi-Fi |
| Sensor VL53L0X | 1 | Sensor de distância por laser |
| LED Verde | 1 | Indicador de status |
| LED Vermelho | 1 | Indicador de erro |
| Resistores 220Ω | 2 | Para os LEDs |
| Jumpers | Vários | Conexões |
| Protoboard | 1 | Opcional |

### Especificações Técnicas
- **Microcontrolador**: ESP32-WROOM-32
- **Sensor**: VL53L0X (alcance até 2m)
- **Comunicação**: Wi-Fi 2.4GHz, I2C
- **Alimentação**: 3.3V
- **Consumo**: ~200mA em operação

## 🔌 Esquema de Ligação

### Diagrama de Ligação
```
ESP32 DOIT DevKit v1
┌─────────────────┐    ┌─────────────────┐
│    ESP32        │    │   VL53L0X       │
│                 │    │                 │
│ Pino 21 (SDA) ──┼────→ SDA             │
│ Pino 22 (SCL) ──┼────→ SCL             │
│ 3.3V        ────┼────→ VCC             │
│ GND         ────┼────→ GND             │
│                 │    │ XSHUT           │
│ Pino 4  (LED G) │    └─────────────────┘
│ Pino 2  (LED R) │    
└─────────────────┘    

Resistores pull-up (recomendados):
• SDA → 3.3V (4.7kΩ)
• SCL → 3.3V (4.7kΩ)
LEDs com resistores de 220Ω
```

### Tabela de Pinagem
| Componente | Pino ESP32 | Função |
|------------|------------|---------|
| VL53L0X SDA | GPIO 21 | I2C Data |
| VL53L0X SCL | GPIO 22 | I2C Clock |
| VL53L0X VCC | 3.3V | Alimentação |
| VL53L0X GND | GND | Terra |
| LED Verde | GPIO 4 | Status |
| LED Vermelho | GPIO 2 | Erro |

## ⚙️ Instalação e Configuração

### Pré-requisitos
- [Arduino IDE](https://www.arduino.cc/en/software) ou [PlatformIO](https://platformio.org/)
- ESP32 Board Package
- Bibliotecas listadas abaixo

### Bibliotecas Necessárias

#### Arduino IDE
```cpp
// Instalar via Library Manager
- WiFi.h (incluída no ESP32)
- HTTPClient.h (incluída no ESP32)
- Wire.h (incluída)
- VL53L0X by Pololu
```

#### PlatformIO (`platformio.ini`)
```ini
[env:doit-devkit-v1]
platform = espressif32
board = doit-devkit-v1
framework = arduino
monitor_speed = 115200

lib_deps = 
    pololu/VL53L0X@^1.3.0
    bblanchon/ArduinoJson@^6.21.3
```

### Configuração da Rede e API

Edite as constantes no código principal:

```cpp
// -- NETWORK AND SERVER CONFIGURATIONS -- //
#define WIFI_SSID       "SUA_REDE_WIFI"
#define WIFI_PASSWORD   "SUA_SENHA_WIFI"
#define SERVER_URL      "SEU_ENDERECO_DE_API"
#define SILO_ID         "SEU_ID_DE_SILO"

// -- Hardware Definitions -- //
#define I2C_SDA_PIN     21
#define I2C_SCL_PIN     22
#define LED_GREEN_PIN   4 
#define LED_RED_PIN     2
```

## 🌐 API e Endpoints

### Estrutura da Requisição
**Endpoint**: `POST https://****SEU_ENDERECO_DE_API****`

**Headers**:
```http
Content-Type: application/json
Connection: close
```

**Payload JSON**:
```json
{
  "silo_id": SILO_ID,
  "level_value": 1250
}
```

### Respostas da API
- `200 OK`: Dados recebidos com sucesso
- `4xx/5xx`: Erro no processamento

## 📁 Estrutura do Projeto

```
silo-monitoring-esp32/
│
├── src/
│   └── main.cpp                 # Código principal
│
├── include/                     # Headers (se necessário)
│
├── lib/                         # Bibliotecas customizadas (se necessário)
│
├── platformio.ini              # Configuração PlatformIO
└── README.md                   # Este arquivo
```

## 🔧 Configuração do Ambiente

### PlatformIO (Recomendado)
1. Instale o PlatformIO IDE
2. Clone este repositório
3. Abra o projeto no PlatformIO
4. Configure o `platformio.ini`
5. Compile e faça upload

## 🚀 Uso e Operação

### Inicialização do Sistema
1. Conecte o hardware conforme o esquema
2. Alimente o sistema
3. Os LEDs piscarão durante a inicialização
4. O sistema conecta automaticamente ao Wi-Fi
5. O sensor inicia as medições

### Comportamento dos LEDs
- **LED Verde Piscando**: Conectando ao Wi-Fi
- **LED Verde Fixo**: Wi-Fi conectado
- **LED Verde Pisca Rápido**: Dados enviados com sucesso
- **LED Vermelho Piscando**: Erro na comunicação
- **LED Vermelho Fixo**: Falha crítica

### Fluxo de Operação
```
Inicialização → Conexão Wi-Fi → Inicialização Sensor → 
Leitura Contínua → Envio API → Aguarda Intervalo → Repete
```

## 📊 Monitoramento e Debug

### Monitor Serial
Conecte via Serial em **9600 baud** para ver logs detalhados:

```bash
🚀 Iniciando sistema de monitoramento do silo...
🌐 Servidor: https://SEU_ENDERECO_DE_API`
Conectando ao Wi-Fi 'SUA_REDE_WIFI'...
....✅ Conectado com sucesso!
📱 IP do ESP32: 192.168.1.100
📡 Iniciando sensor VL53L0X...
✅ Sensor VL53L0X inicializado com sucesso
✅ Sistema pronto para operação
=====================================
📊 {"silo_id": "silo_1", "distancia": 1250, "status": "ok"}
Enviando POST para: https://SEU_ENDERECO_DE_API`
Payload: {"silo_id":"silo_1","level_value":1250}
✅ Resposta HTTP: 200
Resposta do servidor: {"status":"success","id":"12345"}
```

### Logs de Erro Comuns
- `❌ Falha ao inicializar o sensor VL53L0X`: Problema de hardware/I2C
- `❌ Falha ao conectar ao Wi-Fi`: Credenciais incorretas ou rede indisponível
- `⚠️ Timeout na leitura do sensor`: Sensor não responde
- `❌ Erro na requisição`: Problema de conexão com a API

## 🐛 Solução de Problemas

### Problemas Comuns e Soluções

#### Sensor Não Detectado
```bash
# Verifique:
1. Conexões I2C (SDA/SCL)
2. Alimentação 3.3V estável
3. Resistores pull-up (4.7kΩ)
4. Endereço I2C correto (0x29)
```

#### Falha na Conexão Wi-Fi
```bash
# Verifique:
1. SSID e senha corretos
2. Rede 2.4GHz disponível
3. Sinal Wi-Fi suficiente
4. Firewall/restrições de rede
```

#### Erro na Comunicação com API
```bash
# Verifique:
1. URL da API correta
2. Certificado SSL válido
3. Servidor online e acessível
4. Formato do JSON esperado
```

### Modo de Diagnóstico
Para testes, comente a linha do certificado e use:
```cpp
client->setInsecure(); // Apenas para desenvolvimento
```

## 📄 Licença

Este projeto está licenciado sob a Licença MIT - veja o arquivo [LICENSE](LICENSE) para detalhes.

## 📞 Contato

**Desenvolvedor**: Rafhael Gaspar da Silva 
**Email**: rafhael.gaspar.dev@gmail.com

---

## 📈 Próximas Melhorias

- [ ] Interface web para monitoramento
- [ ] Banco de dados local para histórico
- [ ] Configuração via Bluetooth
- [ ] Modo de baixo consumo energético
- [ ] Alertas por email/telegram

## 🏷️ Tags

`ESP32` `IoT` `Agricultura` `Monitoramento` `VL53L0X` `C++` `Arduino` `PlatformIO` `API-REST` `HTTPS`
