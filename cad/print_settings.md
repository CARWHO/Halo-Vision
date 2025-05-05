# 3D Print Settings & Hardware

This document summarizes recommended print parameters, materials, and fasteners for all `/cad/` modules.

---

## 1. Materials & Filament

| Component                                 | Filament Type | Color      | Notes                                     |
| ----------------------------------------- | ------------- | ---------- | ----------------------------------------- |
| Electronics Housing (brainmodule)         | PETG          | Black/Gray | Good chemical resistance                  |
| Display Housing (displaymodule)           | PLA           | White      | Easy printability, aesthetic              |
| Mount Brackets (HaloMount & Direct Mount) | PETG          | Black      | Strong and heat-tolerant                  |
| Hardware (misc)                           | PLA or PETG   | --         | Nuts/spacers printed in PETG for strength |

---

## 2. Printer Settings

| Parameter            | Value                                 | Notes                    |
| -------------------- | ------------------------------------- | ------------------------ |
| Nozzle Diameter      | 0.4 mm                                |                          |
| Layer Height         | 0.2 mm                                | Balance strength & speed |
| Print Speed          | 40–60 mm/s                            | Slower for fine details  |
| Infill               | 20%–30%                               | Rectilinear or grid      |
| Extrusion Temp       | 200–220 °C (PLA)<br>230–250 °C (PETG) | Tune by filament brand   |
| Bed Temp             | 60 °C (PLA)<br>80 °C (PETG)           | Use adhesive if needed   |
| Cooling Fan          | 100% (PLA)<br>50% (PETG)              | Improves overhangs       |
| Build Plate Adhesion | Brim or Raft                          | For small parts & mounts |

---

## 3. Fasteners & Hardware

| Part      | Quantity | Hardware Type       | Notes                    |
| --------- | -------- | ------------------- | ------------------------ |
| M2 Screws | 20       | M2 × 6 mm steel     | Use with printed spacers |
| M3 Screws | 8        | M3 × 10 mm steel    | Secure display bracket   |
| Spacers   | 12       | 6 mm (printed PETG) | For PCB standoffs        |
| Nuts      | 20       | M2 hex nuts         | Nylon-locked preferred   |

> **Tip:** Pre-tap holes with a small drill bit for embedded screws, or print holes 0.1–0.2 mm undersized and thread-tap after printing.

---

## 4. General Notes

* Orient parts to minimize overhangs on critical faces.
* Use supports for internal cavities (e.g., inside housing shells).
* Perform a test fit of all fasteners before final assembly.
* Adjust slicer settings per printer calibration and filament brand.
