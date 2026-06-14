# User Stories for Cocina360 Device (C++ Edition)

This document contains the user stories for the `Cocina360Device` project, focusing exclusively on the end-user personas who interact with the system's physical safety barriers and digital management features.

## End-User Stories

### US01: Real-Time Hierarchical Kitchen Alert System
- **As a** Restaurant Owner, I want the device to continuously monitor temperature and gas levels and automatically deploy visual and acoustic safety barriers, so that my staff and kitchen assets are autonomously protected from fire hazards and gas leaks.
- **Acceptance Criteria**:
  - **Given** the device is powered on and both gas and temperature measurements are within normal boundaries, when the system samples environmental data, then the Green LED remains `ON`, the Yellow/Red LEDs remain `OFF`, and the Buzzer remains silent (SAFE state).
  - **Given** the ambient temperature crosses the Warning threshold ($35^\circ\text{C}$ default) OR the MQ-2 reading crosses the Gas Warning threshold ($1000\text{ PPM}$ default), when the state is evaluated, **then** the Yellow LED turns `ON`, the Green/Red LEDs turn `OFF`, and the system indicates a warning state.
  - **Given** any sensor detects values exceeding the Critical threshold ($50^\circ\text{C}$ or $3000\text{ PPM}$), when the state is updated, **then** the Red LED turns `ON`, the Green/Yellow LEDs turn `OFF`, and the Buzzer asynchronously loops the emergency melody at 175 BPM (**CRITICAL DANGER** state).

### US02: Dynamic Cloud Synchronization of Safety Thresholds
- **As a** Restaurant Owner, **I want** to modify my kitchen's maximum temperature and gas tolerance limits from a backend management portal, **so that** the hardware device automatically recalibrates itself in real time according to my business needs.
- **Acceptance Criteria**:
  - **Given** the device is connected to the restaurant's Wi-Fi network, when the internal clock reaches the 60-second polling interval, **then** the device executes an asynchronous HTTP `GET` request to the cloud configuration endpoint.
  - **Given** the cloud endpoint returns a valid JSON payload containing updated custom thresholds (such as `warnTemp`, `critTemp`, `warnGas`, and `critGas`), when the payload is processed, **then** the device overwrites its in-memory thresholds, immediately shifting the calibration matrix for the sensors.
  - **Given** the cloud network is offline or rate-limited, when the polling executes, **then** the device gracefully falls back to its hardcoded local safety limits, ensuring continuous, uncompromised kitchen protection.

### US03: Real-Time Visual Environmental Feedback
- **As a** Kitchen Staff Member, **I want** a clear, immediate visual indication of the air quality and temperature status on the wall-mounted device, **so that** I can work safely without checking a computer screen or application dashboard.
- **Acceptance Criteria**:
  - **Given** I am working in the kitchen, when I look at the device, **then** a solid Green light reassures me that the environment is completely safe.
  - **Given** a minor gas leak occurs or ventilation fails, when the concentration rises above the warning threshold, **then** the immediate switch to a Yellow light alerts me to turn on the exhaust hoods.
  - **Given** a fire or severe gas leak breaks out, when critical limits are breached, **then** the flashing Red light combined with the loud acoustic buzzer forces an immediate evacuation of the area.

### US04: Automated Cloud Telemetry Logging
- **As a** Restaurant Owner, **I want** the device to automatically send regular status reports and danger alerts to my cloud platform, **so that** I can maintain a historical safety log of my business and receive remote notifications during emergencies.
- **Acceptance Criteria**:
  - **Given** the device is running under normal conditions, when the internal 5-second transmission timer is reached, **then** the system packages the current temperature, PPM values, and status string into a JSON payload and transmits it via HTTP `POST`.
  - **Given** a critical alert event is triggered by the hardware, when the threat level changes, **then** the device prioritizes sending an immediate payload update to the server to notify the manager.

---

*Created: June 14, 2026*
*Last Updated: June 14, 2026*