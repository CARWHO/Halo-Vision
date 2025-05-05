# Halo Vision

**Open-Source Heads-Up Display for Motorcycle Safety**

Halo Vision is a helmet-mounted HUD that delivers turn-by-turn navigation and indicator notifications to motorcyclists, hands-free. Inspired by personal experiences, it evolved from a simple indicator broadcaster into a full display system with custom PCBs and a companion mobile app.

---

## 🚀 What’s in this Repo

- **Schematics**  
  See [`/schematics/README.md`](schematics/README.md).  
  All in `/schematics/`:
  - **Schematic files** (`main_board.kicad_sch`, `indicator_module.sch`) and PCB layouts (`.kicad_pcb`)  
  - **Gerber outputs** in `/schematics/gerbers/` ready for fabrication  
  - **Bill of Materials** (`BOM.csv`) listing every part with distributor links  
  - **Assembly drawings** showing component placement, layer stackup, and silkscreen legends  
  - ATmega32U2 pin mapping and net labels for easy modification  
  ![Schematics overview](\image\pcb.PNG)

- **Software**  
  See [`/software/README.md`](software/README.md).  
  Firmware, drivers, and example hosts to get your HUD talking to GPS and turn signals.  
  ![Software in action](\image\full_view1.PNG)

- **CAD Models & 3D Prints**  
  See [`/cad/README.md`](cad/README.md).  
  All in `/cad/`:
  - **Helmet housing** (`helmet_housing.step`, `helmet_housing.stl`)  
  - **Mount styles**:  
    - `mount_clip.stl` (clip-on)  
    - `mount_screw.stl` (screw-on bracket)  
    - `mount_magnet.stl` (magnetic quick-release)  
  - **Print Guidance** (`/cad/print_settings.md`) lists filament recommendations, print settings, and screw types  
  ![CAD models and mounts](\image\full_view2.PNG)

- **Mobile App**  
  See [`/app/README.md`](app/README.md).  
  Joel Bannister’s Android 13+ navigator app:  
  - **Source**: https://github.com/1ceseismic/android_mapbox_lcd_navigator  
  - **Features**: real-time map with turn arrows, manual indicator override, battery status  
  - **Requirements**: Android 13+, Mapbox GL SDK, Bluetooth LE permissions  
  - **Build & Install**: import into Android Studio, configure Mapbox token, compile APK  
  - **Pairing**: select device `“HaloVisionHUD”`, default PIN `1234`  
  ![Mobile App](\image\mobile_app.PNG)


---

## 🏁 Getting Started

1. **Clone** this repo  
   ```bash
   git clone https://github.com/YourUsername/HaloVision.git
Browse each subfolder’s README for step-by-step build & install instructions.

Flash the firmware, 3D-print your chosen mount, and pair your phone.

🤝 Contributing
This project is 100% open-source. Contributions, issues, and pull requests are warmly welcomed!

📄 License
Distributed under the MIT License. See LICENSE for details.

Email me for further edits: kahu@kora.ac