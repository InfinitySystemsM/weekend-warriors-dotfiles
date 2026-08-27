# 🛡️ Sway WM - Defqon.1 Weekend Warrior (Industrial Dark TUI)

> Configuración modular, ligera y de alto rendimiento para **Sway** (Wayland) con estética industrial inspirada en **Defqon.1 / Hardstyle**, bordes afilados de 1px, acentos en **Warrior Red** (`#e05561`) y **Amber Flame** (`#f38d41`), y herramientas optimizadas para la terminal (TUI).

---

## 🎨 Características Principales

- **Estética Defqon.1 Industrial:** Fondos de asfalto y carbón profundo (`#121317` / `#181a20`) con acentos en rojo guerrero, naranja fuego y destellos de escenario en verde/cian láser. Diseñado cuidadosamente para evitar saturación excesiva o fatiga ocular.
- **Arquitectura Modular de Sway:** Configuración estructurada en `config.d/*.conf` (variables, salidas, entradas, atajos, reglas, scratchpads, autostart, tema y barra).
- **Bordes Afilados de 1px:** Bordes limpios en rojo carmesí para ventanas enfocadas y gris acero para inactivas, con inner gaps de 6px.
- **Waybar Industrial Seamless:** Barra de estado sin bordes ni gaps flotantes con métricas en tiempo real (CPU, RAM, disco, audio, red, batería y reloj integrado).
- **Scratchpads & Popups TUI:** Ventanas flotantes centradas para volumen (`pulsemixer`), monitor (`btop`), selector de redes (`nmtui`), Git (`lazygit`), Bluetooth (`bluetui`), gestor de paquetes (`tui-packages`) y reloj/clima (`tui-calendar`).
- **Lanzadores Dinámicos con Fuzzel:** Menú de apps, selector de portapapeles (`cliphist`), menú de energía (`fuzzel-powermenu`) y selector de Wi-Fi (`fuzzel-wifi`).
- **Terminal Foot & Helix Modal:** Paleta ANSI balanceada de 16 colores industriales y tema dedicado `defqon1.toml` para el editor Helix.

---

## 📂 Estructura del Repositorio

```text
dotfiles/
├── .config/
│   ├── btop/              # Monitor TUI con tema defqon1.theme
│   ├── cava/              # Visualizador de audio con gradiente de fuego
│   ├── environment.d/     # Variables de entorno Wayland
│   ├── fastfetch/         # Resumen del sistema con logos Defqon / Sixel
│   ├── fish/              # Configuración de Fish Shell
│   ├── foot/              # Terminal Wayland con paleta Industrial Dark
│   ├── fuzzel/            # Lanzador de apps y menús dmenu
│   ├── gtk-3.0/ & gtk-4.0/# Configuración de temas GTK oscuros
│   ├── helix/             # Editor modal y tema defqon1.toml
│   ├── ly/                # Configuración estética de Ly Display Manager
│   ├── mako/              # Notificaciones con acentos Defqon
│   ├── micro/             # Editor Micro
│   ├── qt5ct/ & qt6ct/    # Integración visual de aplicaciones Qt
│   ├── spotify-player/    # Reproductor TUI de Spotify
│   ├── starship.toml      # Prompt minimalista con paleta defqon1
│   ├── sway/              # Configuración principal modular de Sway
│   │   ├── config
│   │   └── config.d/      # [01_variables, 02_outputs, 03_inputs, 04_keybindings...]
│   ├── swaylock/          # Pantalla de bloqueo con anillo rojo y efectos
│   ├── waybar/            # Barra superior y hoja de estilos
│   ├── xsettingsd/        # Sincronización XWayland
│   ├── zellij/            # Multiplexor con tema defqon1
│   └── mimeapps.list      # Asociaciones por defecto
├── .local/
│   └── bin/               # Scripts y utilidades personalizadas
│       ├── check-updates     # Verificador JSON de actualizaciones para Waybar
│       ├── fuzzel-cliphist   # Historial del portapapeles interactivo
│       ├── fuzzel-powermenu  # Menú de apagado, reinicio, bloqueo y suspensión
│       ├── fuzzel-wifi       # Selector interactivo de redes Wi-Fi con NetworkManager
│       ├── screenshot-tool   # Utilidad de capturas con selección en rojo carmesí
│       ├── tui-calendar      # Reloj digital gigante, calendario y clima TUI
│       └── tui-packages      # Gestor de paquetes interactivo con FZF (Pacman + Yay)
├── home/
│   ├── .bashrc            # Configuración interactiva de Bash
│   ├── .zshrc             # Configuración interactiva de Zsh
│   └── .gtkrc-2.0         # Compatibilidad de tema GTK2
├── install.sh             # Script automatizado de instalación y symlinks
├── .gitignore
├── LICENSE
└── README.md
```

---

## ⌨️ Atajos de Teclado (Keybindings)

> **Nota:** La tecla `$mod` está configurada como **Super / Windows**.

### 🚀 Lanzamiento de Aplicaciones y Menús
| Atajo | Acción |
|---|---|
| `$mod + Enter` | Abrir terminal (**Foot**) |
| `$mod + Space` / `$mod + d` | Abrir lanzador de aplicaciones (**Fuzzel**) |
| `$mod + v` | Historial del portapapeles (**Fuzzel + Cliphist**) |
| `$mod + y` | Explorador de archivos TUI (**Yazi**) |
| `$mod + Shift + f` | Explorador de archivos GUI (**Thunar**) |
| `$mod + Shift + e` | Menú de energía / sesión (**fuzzel-powermenu**) |
| `$mod + Esc` | Bloquear pantalla (**Swaylock**) |

### 🪟 Scratchpads y Ventanas Flotantes TUI
| Atajo | Herramienta / Pop-up |
|---|---|
| `$mod + m` | Mezclador de audio (**Pulsemixer**) |
| `$mod + Shift + b` | Monitor de recursos (**Btop**) |
| `$mod + Shift + n` | Configuración de red (**NMTUI**) |
| `$mod + Shift + w` | Selector rápido de Wi-Fi (**Fuzzel Wi-Fi**) |
| `$mod + Shift + g` | Interfaz Git (**Lazygit**) |
| `$mod + Shift + t` | Gestor Bluetooth (**Bluetui**) |
| `$mod + i` | Gestor de paquetes (**TUI Packages**) |
| `$mod + c` | Reloj, Calendario y Clima (**TUI Calendar**) |
| `$mod + Shift + Enter` | Terminal flotante scratchpad |
| `$mod + Shift + -` | Enviar ventana activa al scratchpad general |
| `$mod + -` | Alternar/Mostrar scratchpad general |

### 🧭 Navegación y Gestión de Ventanas
| Atajo | Acción |
|---|---|
| `$mod + [h/j/k/l]` o Flechas | Mover foco entre ventanas (Estilo Vim) |
| `$mod + Shift + [h/j/k/l]` | Mover ventana en la dirección indicada |
| `$mod + [1-9, 0]` | Cambiar al espacio de trabajo (Workspace) 1-10 |
| `$mod + Shift + [1-9, 0]` | Mover ventana al espacio de trabajo 1-10 |
| `$mod + f` | Alternar pantalla completa (Fullscreen) |
| `$mod + Shift + Space` | Alternar ventana flotante / tiling |
| `$mod + b` | División horizontal |
| `$mod + n` | División vertical |
| `$mod + s` | Layout en pila (*Stacking*) |
| `$mod + w` | Layout con pestañas (*Tabbed*) |
| `$mod + e` | Alternar división de layout |
| `$mod + r` | Entrar en modo redimensión (*Resize mode*) |
| `$mod + Shift + q` | Cerrar ventana enfocada |
| `$mod + Shift + c` | Recargar configuración de Sway al instante |

### 📸 Capturas de Pantalla
| Atajo | Tipo de Captura |
|---|---|
| `Print` | Captura de pantalla completa al portapapeles |
| `Shift + Print` o `$mod + Shift + s` | Selección de área interactiva con `slurp` |
| `$mod + Print` | Captura de la ventana activa enfocada |

### 🔊 Controles de Medios y Brillo
| Atajo | Acción |
|---|---|
| `XF86AudioMute` | Silenciar / Activar audio |
| `XF86AudioLowerVolume` | Bajar volumen (-5%) |
| `XF86AudioRaiseVolume` | Subir volumen (+5%) |
| `XF86AudioMicMute` | Silenciar / Activar micrófono |
| `XF86AudioPlay` / `Pause` | Reproducir / Pausar medios |
| `XF86AudioNext` / `Prev` | Siguiente / Anterior pista |
| `XF86MonBrightnessUp/Down` | Ajustar brillo de pantalla (+/- 5%) |

---

## 📦 Paquetes y Dependencias

Para instalar todas las dependencias en **Arch Linux** o **CachyOS**:

```bash
sudo pacman -S --needed \
    sway waybar fuzzel mako swaylock foot grim slurp wl-clipboard \
    cliphist playerctl brightnessctl libnotify polkit-gnome starship \
    btop helix micro zellij cava fastfetch fish yazi thunar \
    adw-gtk-theme papirus-icon-theme qt5ct qt6ct xsettingsd \
    ttf-meslo-nerd networkmanager pulsemixer
```

Y vía **AUR / Yay**:

```bash
yay -S --needed swaylock-effects bluetui lazygit spotify-player
```

---

## 🛠️ Instalación y Uso de Dotfiles

1. **Clonar este repositorio en tu directorio personal:**
   ```bash
   git clone <URL_DE_TU_REPOSITORIO> ~/dotfiles
   cd ~/dotfiles
   ```

2. **Ejecutar el script de instalación:**
   ```bash
   ./install.sh
   ```

3. **Recargar Sway:**
   Presiona `$mod + Shift + c` para aplicar todos los cambios inmediatamente.

---

## 📄 Licencia

Distribuido bajo la Licencia MIT. Consulta el archivo [LICENSE](file:///home/infinity/dotfiles/LICENSE) para más información.
