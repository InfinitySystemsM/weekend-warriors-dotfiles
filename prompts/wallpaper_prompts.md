# 🌌 Prompts de Fondos de Pantalla - Defqon.1 Weekend Warrior

Este documento recopila los prompts de Inteligencia Artificial diseñados para generar fondos de pantalla que combinan la estética topográfica e industrial con el emblema de **Defqon.1**.

---

## 🎨 Prompt Principal: Topografía Industrial Oscura (16:9 / 4K)

> **Modelo recomendado:** Midjourney v6, DALL-E 3, Flux.1, Imagen 3

### Prompt en Inglés (Óptimo para IA):
```text
A sleek, minimalist dark industrial desktop wallpaper in 16:9 aspect ratio, 4K resolution. The background features dark charcoal (#121317) and deep matte asphalt 3D topographic contour elevation curves with a subtle brushed metallic sheen and soft shadows. In the exact center, display the iconic geometric Defqon.1 hardstyle emblem logo, rendered as a matte black and dark steel plate with a subtle, warm warrior crimson red (#e05561) and amber flame (#f38d41) inner rim glow. Minimalist hardstyle aesthetic, elegant dark mode composition, sharp details, zero noise, high contrast, clean industrial aesthetic.
```

### Prompt en Español:
```text
Fondo de pantalla de escritorio industrial oscuro y minimalista en proporción 16:9 y resolución 4K. El fondo presenta curvas topográficas 3D en negro mate y carbón oscuro (#121317) con una textura metálica cepillada sutil y sombras suaves. En el centro exacto, el icónico logo geométrico de Defqon.1 en acabado acero oscuro mate con un resplandor tenue en los bordes en rojo carmesí guerrero (#e05561) y naranja fuego (#f38d41). Estética hardstyle elegante, composición limpia para modo oscuro y máximo contraste sin saturación excesiva.
```

---

## ⚡ Variante 2: Escenario y Láseres Hardstyle Minimalista

```text
Minimalist dark hardstyle festival stage background, 16:9 aspect ratio, 4K. Deep matte black background with subtle industrial steel grid scaffolding and faint atmospheric haze. In the center, a floating glowing metallic Defqon.1 logo with sharp crimson (#e05561) and stage cyan (#56b6c2) laser accents slicing through the dark atmosphere. Clean geometry, cinematic lighting, zero clutter, wallpaper suitable for desktop.
```

---

## 🛠️ Cómo Aplicar un Nuevo Fondo en Sway

1. Coloca tu nueva imagen en `~/.config/sway/wallpapers/tu_fondo.jpg`.
2. Actualiza `~/.config/sway/config.d/02_outputs.conf`:
   ```sway
   output * bg ~/.config/sway/wallpapers/tu_fondo.jpg fill #121317
   ```
3. Recarga Sway con `$mod + Shift + c` o aplica en vivo con:
   ```bash
   swaymsg "output * bg ~/.config/sway/wallpapers/tu_fondo.jpg fill #121317"
   ```
