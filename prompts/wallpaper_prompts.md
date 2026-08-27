# 🌌 Prompts de Fondos de Pantalla - Defqon.1 Weekend Warrior

Este documento recopila los prompts de Inteligencia Artificial diseñados para generar fondos de pantalla que combinan la estética industrial, geométrica y el emblema de **Defqon.1**.

---

## 📐 Prompt 1: Geométrico Industrial Plano (Líneas Rectas y Circuitos • #0f1013)

> **Actual fondo predeterminado del sistema.** Sin degradados pesados, fondo en asfalto plano idéntico a Waybar y líneas angulares a 45°/90°.

### Prompt en Inglés:
```text
A flat, minimalist, dark industrial desktop wallpaper in 16:9 aspect ratio, 4K resolution. The entire background is a solid, flat, deep dark charcoal asphalt (#0f1013) with no vignette and no heavy lighting gradients. Across the background, there is an intricate pattern of razor-sharp straight lines, 45-degree and 90-degree angular isometric geometric gridlines, circuit blueprint schematic lines in subtle muted steel gray (#222530) and faint laser dark red. In the exact center, the geometric Defqon.1 emblem logo is integrated cleanly with sharp, clean lines in white and warrior crimson red (#e05561). Flat, clean, razor-sharp industrial hardstyle aesthetic.
```

### Prompt en Español:
```text
Fondo de pantalla de escritorio industrial oscuro, plano y minimalista en proporción 16:9 y resolución 4K. Todo el fondo es un asfalto carbón oscuro plano y sólido (#0f1013) sin viñeteado ni degradados de luz pesados. A través del fondo, un patrón intrincado de líneas rectas nítidas, cuadrículas geométricas isométricas con ángulos de 45° y 90°, y líneas de esquemático de circuitos en gris acero tenue (#222530) y rojo láser sutil. En el centro exacto, el logo geométrico de Defqon.1 integrado con líneas limpias en blanco y rojo carmesí guerrero (#e05561). Estética hardstyle industrial plana y nítida.
```

---

## 🎨 Prompt 2: Topografía Industrial 3D

```text
A sleek, minimalist dark industrial desktop wallpaper in 16:9 aspect ratio, 4K resolution. The background features dark charcoal (#121317) and deep matte asphalt 3D topographic contour elevation curves with a subtle brushed metallic sheen and soft shadows. In the exact center, display the iconic geometric Defqon.1 hardstyle emblem logo, rendered as a matte black and dark steel plate with a subtle, warm warrior crimson red (#e05561) and amber flame (#f38d41) inner rim glow. Minimalist hardstyle aesthetic, elegant dark mode composition, sharp details, zero noise.
```

---

## 🛠️ Cómo Cambiar el Fondo en Sway

1. Coloca tu imagen en `~/.config/sway/wallpapers/`.
2. Edita `~/.config/sway/config.d/02_outputs.conf`:
   ```sway
   output * bg ~/.config/sway/wallpapers/defqon_geometric.jpg fill #0f1013
   ```
3. Aplica en vivo con:
   ```bash
   swaymsg "output * bg ~/.config/sway/wallpapers/defqon_geometric.jpg fill #0f1013"
   ```
