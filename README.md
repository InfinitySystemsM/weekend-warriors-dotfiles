# 🌌 Sway WM - Catppuccin Mocha TUI Edition

> Configuración modular, ligera y estética de **Sway** (Wayland) orientada a herramientas TUI (*Terminal User Interface*), con bordes nítidos de 1px, paleta **Catppuccin Mocha** y flujo de trabajo altamente productivo.

---

## 🎨 Características Principales

- **Arquitectura Modular de Sway:** Configuración dividida limpiamente en `config.d/*.conf` (variables, salidas, entradas, atajos, reglas, scratchpads, autostart, tema y barra).
- **Estética Catppuccin Mocha:** Colores coherentes en Sway, Waybar, Fuzzel, Foot, Mako, Swaylock, Starship, Helix, Zellij, Btop, Cava, GTK y Qt.
- **Bordes Nítidos de 1px & Gaps:** Sin bordes redondeados innecesarios, bordes de 1px con acento Mauve (`#cba6f7`) e inner gaps de 6px.
- **Waybar TUI Seamless:** Barra superior sin bordes ni gaps flotantes, integrada con módulos de estado en tiempo real (reloj, CPU, RAM, disco, audio, red, batería y clima/calendario).
- **Scratchpads & Popups TUI Flotantes:** Ventanas emergentes centradas para control de volumen (`pulsemixer`), monitor del sistema (`btop`), selector de redes (`nmtui`), Git (`lazygit`), Bluetooth (`bluetui`), gestor de paquetes (`tui-packages`) y calendario interactivo con clima (`tui-calendar`).
- **Lanzadores Dinámicos con Fuzzel:** Menú de aplicaciones, selector de portapapeles (`cliphist`), menú de apagado/bloqueo (`fuzzel-powermenu`) y selector interactivo de Wi-Fi (`fuzzel-wifi`).
- **Capturas Rápidas con Notificaciones:** Herramienta integrada con `grim`, `slurp` y `wl-copy` que copia directo al portapapeles y notifica al usuario.

---

## 📂 Estructura del Repositorio

```text
dotfiles/
├── .config/
│   ├── btop/              # Monitor del sistema TUI
│   ├── cava/              # Visualizador de espectro de audio (shaders & themes)
│   ├── environment.d/     # Variables de entorno para Wayland y temas oscuros
│   ├── fastfetch/         # Resumen del sistema con logos Sixel / Defqon
│   ├── fish/              # Configuración de Fish Shell
│   ├── foot/              # Emulador de terminal nativo de Wayland
│   ├── fuzzel/            # Lanzador de aplicaciones y menús dmenu
│   ├── gtk-3.0/           # Configuración de temas GTK3 (adw-gtk3-dark)
│   ├── gtk-4.0/           # Configuración de temas GTK4
│   ├── helix/             # Editor modal moderno con tema Catppuccin
│   ├── ly/                # Configuración estética de Ly Display Manager
│   ├── mako/              # Demonio de notificaciones ligeras
│   ├── micro/             # Editor de texto de terminal y atajos
│   ├── qt5ct/ & qt6ct/    # Integración visual de aplicaciones Qt
│   ├── spotify-player/    # Reproductor TUI de Spotify
│   ├── starship.toml      # Prompt minimalista y rápido para la terminal
│   ├── sway/              # Configuración principal modular de Sway
│   │   ├── config
│   │   └── config.d/      # [01_variables, 02_outputs, 03_inputs, 04_keybindings...]
│   ├── swaylock/          # Pantalla de bloqueo con efectos de desenfoque y reloj Ly
│   ├── waybar/            # Barra de estado superior y hoja de estilos CSS
│   ├── xsettingsd/        # Sincronización de temas y fuentes X11/XWayland
│   ├── zellij/            # Multiplexor de terminales con pestañas y paneles
│   └── mimeapps.list      # Asociaciones de archivos por defecto
├── .local/
│   └── bin/               # Scripts y utilidades personalizadas
│       ├── check-updates     # Verificador JSON de actualizaciones para Waybar
│       ├── fuzzel-cliphist   # Historial del portapapeles interactivo
│       ├── fuzzel-powermenu  # Menú de apagado, reinicio, bloqueo y suspensión
│       ├── fuzzel-wifi       # Selector interactivo de redes Wi-Fi con NetworkManager
│       ├── screenshot-tool   # Utilidad de capturas (pantalla completa, área, ventana)
│       ├── tui-calendar      # Reloj digital gigante, calendario y clima TUI
│       └── tui-packages      # Gestor interactivo de paquetes con FZF (Pacman + Yay)
├── home/
│   ├── .bashrc            # Configuración interactiva de Bash
│   ├── .zshrc             # Configuración interactiva de Zsh
│   └── .gtkrc-2.0         # Compatibilidad de tema para aplicaciones GTK2
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

Para instalar todas las dependencias en **Arch Linux** o **CachyOS**, puedes ejecutar:

```bash
sudo pacman -S --needed \
    sway waybar fuzzel mako swaylock foot grim slurp wl-clipboard \
    cliphist playerctl brightnessctl libnotify polkit-gnome starship \
    btop helix micro zellij cava fastfetch fish yazi thunar \
    adw-gtk-theme papirus-icon-theme qt5ct qt6ct xsettingsd \
    ttf-meslo-nerd networkmanager pulsemixer
```

Y para utilidades TUI adicionales vía **AUR / Yay**:

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
   > El script creará enlaces simbólicos (`symlinks`) automáticos hacia `~/.config`, `~/.local/bin` y `~`, haciendo una copia de seguridad previa de cualquier configuración existente en `~/.dotfiles_backup/`.

3. **Opciones adicionales del instalador:**
   ```bash
   ./install.sh --copy         # Copia los archivos en vez de crear enlaces simbólicos
   ./install.sh --check-deps   # Revisa si faltan paquetes y permite instalarlos
   ./install.sh --no-backup    # Omite el respaldo de archivos existentes
   ```

4. **Recargar Sway:**
   Presiona `$mod + Shift + c` para aplicar todos los cambios inmediatamente.

---

## 📤 Instrucciones para Subir a GitHub

Si deseas subir este repositorio a tu cuenta de GitHub:

1. Crea un nuevo repositorio vacío en GitHub (por ejemplo, con el nombre `dotfiles` o `sway-dotfiles`).
2. En tu terminal, ejecuta los siguientes comandos dentro de la carpeta:

```bash
cd ~/dotfiles
git remote add origin git@github.com:TU_USUARIO/dotfiles.git
# o mediante HTTPS:
# git remote add origin https://github.com/TU_USUARIO/dotfiles.git

git branch -M main
git push -u origin main
```

---

## 📄 Licencia

Distribuido bajo la Licencia MIT. Consulta el archivo [LICENSE](file:///home/infinity/dotfiles/LICENSE) para más información.
