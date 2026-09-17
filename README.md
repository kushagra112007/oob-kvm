# Zero-Cloud Out-of-Band (OOB) Laptop KVM & Power Node


> **45-Second Demo Video:** 


https://github.com/user-attachments/assets/e6dacda2-8d32-48b7-a407-9e057e0ee7ed
<img width="2376" height="1080" alt="IMG_20260917_153302" src="https://github.com/user-attachments/assets/a2a33a8c-9df5-4d91-a169-62ca48370bec" />



An ultra-low-cost (~$6.90 USD) hardware out-of-band management solution designed for headless laptop servers lacking Wake-on-LAN support. Features non-blocking mechanical power button actuation, zero-cloud REST API control, multi-tier SSH snippet integration, offline IR air-gapped fallback, and pre-boot optical KVM telemetry.


---

##  Key Features

* **Physical Button Actuation:** Overrides ACPI S5 and Wake-on-LAN failure states using a calibrated SG90 micro-servo.
* **Smooth Sweep C++ Engine:** Custom pulse-width calibration (500–2400µs) with non-blocking `yield()` loops prevents Watchdog (WDT) resets, gear stripping, and base lift.
* **REST API & IR Control:** Network endpoints for `/wake`, `/power`, and `/panic`, backed by an offline IR remote module for local air-gapped overrides.
* **Pre-Boot Optical KVM:** Repurposed Android camera bridge running **IP Webcam** to capture raw POST/BIOS screen display output before OS GPU drivers load.
* **Multi-Tier Secure Access:** Zero-exposure networking routed globally over WireGuard / Tailscale mesh tunnels (CGNAT bypass).
* **Automated WAN Failover:** Edge gateway logic on the phone automatically switches to a 4G LTE hotspot during home broadband outages (~20s recovery).

---

## Hardware Architecture & Pre-Boot Telemetry

Most laptop motherboards disable HDMI, DisplayPort, and Type-C outputs during initial execution (POST, BIOS/UEFI, early kernel boot), enabling external ports only after OS display drivers load.

To maintain continuous visual monitoring before the OS boots, an external smartphone camera running **IP Webcam** pointed at the primary laptop panel acts as an out-of-band optical capture bridge.

### Bill of Materials (BOM)

| Component | Quantity | Cost (USD) | Function / Notes |
| :--- | :---: | :---: | :--- |
| **NodeMCU ESP8266** | 1 | $4.20 | REST Web Server & Actuator Controller |
| **SG90 Micro Servo** | 1 | $1.14 | Physical power button actuator |
| **IR Remote + Receiver Module** | 1 | $0.84 | Offline air-gapped manual fallback override |
| **Micro-USB Power Cable** | 1 | $0.60 | Power delivery to NodeMCU |
| **Jumper Wires** | Pack | $0.12 | Signal & GPIO power routing |
| **Asus ZenFone (2018)** | 1 | $0.00 | E-waste recovery: Optical POST/BIOS camera bridge & SSH Gateway Bridge |
| **Tenda Router** | 1 | $0.00 | E-waste recovery: Local subnet AP / MAC-to-IP reservation node |
| **Wooden Platter Mount** | 1 | $0.00 | Repurposed scrap mount base |
| **3D Printed Arm & Bracket** | 1 | $0.00 | Custom STL lever extension (College lab perk) |
| **TOTAL** | | **~$6.90** | **Complete OOB-KVM Node** |

---

###  Execution Steps

1. **Trigger Delivery:** Initiate an SSH snippet or cURL payload from your remote client (OnePlus / Termius) over the encrypted Tailscale mesh tunnel to the gateway phone (`100.x.y.z:8022`).
2. **Local Payload Dispatch:** The gateway phone receives the ingress packet and executes an internal shell alias that issues an HTTP GET request across the isolated Tenda subnet (`192.168.0.x`) targeting the NodeMCU endpoint.
3. **Hardware Switch Actuation:** The NodeMCU processes the REST call (`/wake`, `/power`, or `/panic`), sweeps the SG90 micro-servo to depress the physical ACPI power button, and automatically resets to its neutral 180° resting state.
4. **Visual Telemetry Verification:** If OS-level remote access (SSH/RDP) fails or hangs, stream the IP Webcam feed to inspect live pre-boot POST output, BIOS menus, or kernel panic crash logs.
---





##  Network Topology & Control Workflow

The setup isolates management traffic on a dedicated sub-router subnet to guarantee static routing and prevent IP drift across power cycles.

### Static Subnet Reservations (Tenda AP - `192.168.1.x`)
* **Target Server Host (`cachyos-x86_64`):** Reserved at `192.168.1.100`
* **Gateway Phone Bridge:** Reserved at `192.168.1.101`
* **NodeMCU ESP8266 Actuator:** Reserved at `192.168.1.102` (Static DHCP Lease)
---


| Layer | Transport Channel | Range / Context | Operating Requirements | Failure Mitigation Target |
| :--- | :--- | :--- | :--- | :--- |
| **Primary (Global OOB)** | Tailscale WireGuard + SSH | Global (Cellular / 5G) | Cellular Data + Gateway Phone Active | Worldwide server recovery & optical screen capture |
| **Secondary (Local LAN)** | HTTP REST API over Local Wi-Fi | Local Subnet Radius | Local Tenda AP Active | Bypasses WAN internet outages or Tailscale relay downtime |
| **Tertiary (Air-Gap)** | 38kHz Infrared Signal (GPIO D2) | Physical Desk Radius (Line-of-Sight) | 5V Power to NodeMCU Only | Functions when Wi-Fi routers, SSH bridges, and local subnets fail entirely |




```text
[ Remote Client (Mobile / 5G) ]
       │
       ▼ (Encrypted WireGuard Mesh via Tailscale)
[ Gateway Phone Bridge (100.x.y.z:8022) ] ──> IP Webcam Stream / Video Capture
       │
       ▼ (Local HTTP Payload / cURL Script across Tenda Subnet)
[ NodeMCU Actuator (192.168.1.102) ]
       │
       ▼ (Sweeps SG90 Servo to depress power key)
[ Target Server (`cachyos-x86_64` @ 192.168.1.100) ]
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
