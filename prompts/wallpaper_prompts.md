# 🌌 Prompts de Fondos de Pantalla - Defqon.1 Weekend Warrior

Este documento recopila los prompts de Inteligencia Artificial diseñados para generar fondos de pantalla que combinan la estética industrial, geométrica y el emblema de **Defqon.1**.

---

## 📐 Prompt 1: Geométrico Industrial Plano (Líneas Rectas y Circuitos • #0e1015)

> Fondo en asfalto oscuro profundo con líneas angulares y acentos escarlata.

### Prompt en Inglés:
```text
A flat, minimalist, dark industrial desktop wallpaper in 16:9 aspect ratio, 4K resolution. The entire background is a solid, flat, deep dark charcoal obsidian (#0e1015) with no vignette and no heavy lighting gradients. Across the background, there is an intricate pattern of razor-sharp straight lines, 45-degree and 90-degree angular isometric geometric gridlines, circuit blueprint schematic lines in subtle muted steel gray (#282c37) and faint laser dark red. In the exact center, the geometric Defqon.1 emblem logo is integrated cleanly with sharp, clean lines in white and Defqon crimson red (#dc2626). Flat, clean, razor-sharp industrial hardstyle aesthetic.
```

### Prompt en Español:
```text
Fondo de pantalla de escritorio industrial oscuro, plano y minimalista en proporción 16:9 y resolución 4K. Todo el fondo es un asfalto carbón oscuro plano y sólido (#0e1015) sin viñeteado ni degradados de luz pesados. A través del fondo, un patrón intrincado de líneas rectas nítidas, cuadrículas geométricas isométricas con ángulos de 45° y 90°, y líneas de esquemático de circuitos en gris acero tenue (#282c37) y rojo láser sutil. En el centro exacto, el logo geométrico de Defqon.1 integrado con líneas limpias en blanco y rojo carmesí Defqon (#dc2626). Estética hardstyle industrial plana y nítida.
```

---

## 🎨 Prompt 2: Topografía Industrial 3D

```text
A sleek, minimalist dark industrial desktop wallpaper in 16:9 aspect ratio, 4K resolution. The background features dark charcoal (#0e1015) and deep matte asphalt 3D topographic contour elevation curves with a subtle brushed metallic sheen and soft shadows. In the exact center, display the iconic geometric Defqon.1 hardstyle emblem logo, rendered as a matte black and dark steel plate with a subtle, warm Defqon crimson red (#dc2626) and solar gold (#f59e0b) inner rim glow. Minimalist hardstyle aesthetic, elegant dark mode composition, sharp details, zero noise.
```

---

## 🔥 Prompt 3: El Altar Sagrado del RED Mainstage (Fuego, Acero y Deidad Guerrera)

> **Inspirado directamente en las fotos del escenario RED de Defqon.1.** Altar simétrico monumental, deidad solar dorada, espadas ceremoniales y lanzallamas pirotécnicos verticales.

### Prompt en Inglés:
```text
Epic 4K wallpaper inspired by Defqon.1 RED Mainstage. Monumental symmetrical hardstyle warrior altar with a colossal robotic tribal warrior deity face in burnished molten gold and crimson lacquer (#dc2626). Flanked by towering sharp mechanical sword spires and curved demon blade wings in deep obsidian dark steel (#0e1015) and blood scarlet. Massive vertical flame cannons blasting blazing orange (#f97316) and incandescent yellow (#facc15) fire pillars into a dark smoky night sky with subtle laser cyan (#06b6d4) and electric magenta stage beams. Dark, clean, atmospheric desktop wallpaper, highly detailed mechanical sculpture.
```

### Prompt en Español:
```text
Fondo de pantalla épico 4K inspirado en el escenario principal RED de Defqon.1. Altar de guerrero hardstyle monumental y simétrico con un rostro colosal de deidad guerrera tribal robótica en oro fundido bruñido y laca carmesí (#dc2626). Flanqueado por imponentes agujas en forma de espadas mecánicas afiladas y alas de cuchillas curvadas en acero oscuro obsidiana (#0e1015) y escarlata intenso. Enormes cañones de fuego verticales disparando columnas de llamas naranja ardiente (#f97316) y amarillo incandescente (#facc15) hacia un cielo nocturno con humo sutil y haces de láser cian (#06b6d4) y magenta eléctrico. Fondo de escritorio oscuro, limpio y atmosférico.
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
