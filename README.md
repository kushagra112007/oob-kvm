# Zero-Cloud Out-of-Band (OOB) Laptop KVM & Power Node

An ultra-low-cost (~$8.25 USD) hardware out-of-band management solution designed for headless laptop servers lacking Wake-on-LAN support. Features mechanical power button actuation, zero-cloud REST API control, mobile SSH snippet integration, and pre-boot optical KVM telemetry.

<img width="3039" height="1823" alt="IMG20260831220534" src="https://github.com/user-attachments/assets/75f2f5c5-110a-4858-82fb-66a714e70b1b" />


---

## Key Features

* **Physical Actuation:** Overrides ACPI S5 and Wake-on-LAN failure states using an SG90 servo motor.
* **REST API Control:** Dedicated endpoints for `/wake` (1.5s), `/power` (5s), and `/panic` (15s hard cold-boot reset).
* **Pre-Boot Optical KVM:** Repurposed Android camera running Termux to capture raw display output before OS drivers load.
* **Multi-Tier Secure Access:** Zero-exposure networking routed globally over WireGuard / Tailscale mesh tunnels.

---

## Hardware Architecture & Pre-Boot Telemetry

Most laptop motherboards disable HDMI, DisplayPort, and Type-C display outputs during initial execution (POST, BIOS/UEFI screens, and early kernel initialization), turning on external output ports only after OS display drivers load.

To maintain continuous visual monitoring before the OS boots, an external phone camera pointed at the primary laptop panel acts as an out-of-band optical capture bridge.

### Bill of Materials (BOM)

| Component | Quantity | Cost (USD) | Function / Notes |
| :--- | :---: | :---: | :--- |
| NodeMCU ESP8266 | 1 | $3.25 | REST Web Server & Actuator Controller |
| SG90 Micro Servo | 1 | $3.00 | Physical power button actuation |
| Old Android Smartphone (2018 Asus ZenFone) | 1 | $0.00 | E-waste recovery: Optical KVM pre-boot capture via Termux |
| Desktop Boom Arm Setup | 1 | $0.00 | E-waste recovery: Repurposed lamp holder for counterweight mount |
| Jumper Cables | Pack of 4 | $2.00 | GPIO signal & power routing |
| **Total** | | **~$8.25** | |

---

## 🔧 REST API Endpoints

```text
GET /wake     -> Triggers 1.5-second display/sleep toggle press
GET /power    -> Triggers 5.0-second standard power-on hold
GET /panic    -> Triggers 15.0-second hard kernel freeze power cycle
GET /release  -> Resets servo back to 0° rest position
```

---

## License & Community

Featured in the top 10 projects on r/techsupportmacgyver (July 2026).  
Distributed under the MIT License.
