The following PlantUML diagram illustrates the classes of the Cocina360 Device (C++ Edition), spanning both the Modest IoT Nano-framework core (`Command`, `Event`, `CommandHandler`, `EventHandler`, `Device`, `Sensor`, `Actuator`) and the project-specific classes (`Dht11Sensor`, `Mq2Sensor`, `Led`, `Buzzer`, `ServoActuator`, `Cocina360Device`), along with their attributes, methods, and relationships. `Cocina360Device` also aggregates a `PubSubClient` (MQTT) connection used to deliver remote actuator commands, such as toggling the `ServoActuator`.

```plantuml
@startuml

top to bottom direction

struct Command {
    int id
    +Command(int commandId)
    +operator==(const Command& other) : bool
}

struct Event {
    int id
    +Event(int eventId)
    +operator==(const Event& other) : bool
}

abstract class CommandHandler {
    +handle(Command command) = 0
}

abstract class EventHandler {
    +on(Event event) = 0
}

abstract class Device {
    +on(Event event) = 0
    +handle(Command command) = 0
}
Device -up-|> EventHandler
Device -up-|> CommandHandler

class Sensor {
    protected int pin
    protected EventHandler* handler
    +Sensor(int pin, EventHandler* eventHandler = nullptr)
    +on(Event event)
}
Sensor -up-|> EventHandler

class Actuator {
    protected int pin
    protected CommandHandler* handler
    +Actuator(int pin, CommandHandler* commandHandler = nullptr)
    +handle(Command command)
}
Actuator -up-|> CommandHandler

class Dht11Sensor {
    private DHT dht
    private unsigned long lastReadTime
    private int currentState
    private int lastTemperature
    +static const int TEMP_NORMAL_EVENT_ID = 30
    +static const int TEMP_WARN_EVENT_ID = 31
    +static const int TEMP_CRIT_EVENT_ID = 32
    +static const Event TEMP_NORMAL_EVENT
    +static const Event TEMP_WARN_EVENT
    +static const Event TEMP_CRIT_EVENT
    +Dht11Sensor(int pin, EventHandler* eventHandler = nullptr)
    +begin() : void
    +update() : void
    +getLatestTemperature() : int
}
Dht11Sensor -up-|> Sensor

class Mq2Sensor {
    private unsigned long lastReadTime
    private int lastADC
    private float lastPPM
    private int currentState
    +static const int GAS_NORMAL_EVENT_ID = 20
    +static const int GAS_WARN_EVENT_ID = 21
    +static const int GAS_CRIT_EVENT_ID = 22
    +static const Event GAS_NORMAL_EVENT
    +static const Event GAS_WARN_EVENT
    +static const Event GAS_CRIT_EVENT
    +Mq2Sensor(int pin, EventHandler* eventHandler = nullptr)
    +update() : void
    +getLatestPPM() : float
}
Mq2Sensor -up-|> Sensor

class Led {
    private bool state
    +static const int TOGGLE_LED_COMMAND_ID = 0
    +static const int TURN_ON_COMMAND_ID = 1
    +static const int TURN_OFF_COMMAND_ID = 2
    +static const Command TOGGLE_LED_COMMAND
    +static const Command TURN_ON_COMMAND
    +static const Command TURN_OFF_COMMAND
    +Led(int pin, bool initialState = false, CommandHandler* commandHandler = nullptr)
    +handle(Command command) override
    +getState() : bool
    +setState(bool newState) : void
}
Led -up-|> Actuator

class Buzzer {
    private bool isPlaying
    private unsigned long noteStartTime
    private int currentNote
    +static const int PLAY_ALARM_COMMAND_ID = 10
    +static const int STOP_ALARM_COMMAND_ID = 11
    +static const Command PLAY_ALARM_COMMAND
    +static const Command STOP_ALARM_COMMAND
    +Buzzer(int pin, CommandHandler* commandHandler = nullptr)
    +handle(Command command) override
    +update() : void
}
Buzzer -up-|> Actuator

class ServoActuator {
    private Servo servo
    private bool isOn
    private int pin
    private int currentAngle
    private int step
    private unsigned long lastUpdate
    private unsigned long lastToggleTime
    +static const int TOGGLE_SERVO_COMMAND_ID = 40
    +static const Command TOGGLE_SERVO_COMMAND
    +ServoActuator(int pin, CommandHandler* commandHandler = nullptr)
    +handle(Command command) override
    +update() : void
    +getState() : bool
}
ServoActuator -up-|> Actuator

class Cocina360Device {
    private WiFiClient espClient
    private PubSubClient mqttClient
    private Dht11Sensor dhtSensor
    private Mq2Sensor gasSensor
    private Led redLed
    private Led yellowLed
    private Led greenLed
    private Buzzer buzzer
    private ServoActuator servoDisipador
    private int tempSeverity
    private int gasSeverity
    private int warnTemperatureC
    private int critTemperatureC
    private float warnGasPpm
    private float critGasPPM
    -const char* ssid
    -const char* password
    -const String deviceId
    -const String deviceApiKey
    -const String edgeServerIp
    -const String urlConfigGet
    -const String urlTelemetryPost
    -const char* mqttServer
    -const int mqttPort
    -String commandTopic
    -unsigned long lastFetchTime
    -const unsigned long FETCH_INTERVAL_MS
    -evaluateGlobalState() : void
    -connectWiFi() : void
    -reconnectMQTT() : void
    -fetchRemoteThresholds() : void
    -sendTelemetry(int temp, float ppm, String status) : void
    -{static} mqttCallback(char* topic, byte* payload, unsigned int length) : void
    +static const int PIN_DHT = 19
    +static const int PIN_MQ2 = 32
    +static const int PIN_RED = 25
    +static const int PIN_YELLOW = 26
    +static const int PIN_GREEN = 33
    +static const int PIN_BUZZER = 14
    +static const int PIN_SERVO = 27
    +Cocina360Device()
    +begin() : void
    +update() : void
    +on(Event event) override
    +handle(Command command) override
    +getWarnTemp() : int
    +getCritTemp() : int
    +getWarnGas() : float
    +getCritGas() : float
}
Cocina360Device -up-|> Device

Cocina360Device *-- Dht11Sensor : instantiates
Cocina360Device *-- Mq2Sensor : instantiates
Cocina360Device *-- Led : instantiates (x3)
Cocina360Device *-- Buzzer : instantiates
Cocina360Device *-- ServoActuator : instantiates
Cocina360Device --> "1" PubSubClient : mqttClient (receives remote commands)

@enduml
```
