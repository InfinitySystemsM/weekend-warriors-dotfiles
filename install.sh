#!/usr/bin/env bash
# ==============================================================================
# Dotfiles Installation & Symlink Script
# Sway Defqon.1 Weekend Warrior - Industrial Dark TUI
# ==============================================================================

set -e

DOTFILES_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BACKUP_DIR="$HOME/.dotfiles_backup/$(date +%Y%m%d_%H%M%S)"

# Colors for output
C_RESET="\033[0m"
C_RED="\033[1;31m"
C_BLUE="\033[1;34m"
C_CYAN="\033[1;36m"
C_GREEN="\033[1;32m"
C_YELLOW="\033[1;33m"

banner() {
    echo -e "${C_RED}"
    echo "  ╔════════════════════════════════════════════════════════════════╗"
    echo "  ║        Sway Defqon.1 Industrial TUI - Dotfiles Installer          ║"
    echo "  ╚════════════════════════════════════════════════════════════════╝"
    echo -e "${C_RESET}"
}

usage() {
    echo "Uso: $0 [OPCIONES]"
    echo ""
    echo "Opciones:"
    echo "  --symlink     Crea enlaces simbólicos (por defecto)"
    echo "  --copy        Copia los archivos en lugar de crear enlaces simbólicos"
    echo "  --no-backup   No realiza copia de seguridad de los archivos existentes"
    echo "  --check-deps    Verifica e instala dependencias del sistema (Arch / CachyOS)"
    echo "  --with-tuicast  Compila e instala TuiCast (controlador TUI de OBS) si existe ~/tuicast"
    echo "  -h, --help      Muestra esta ayuda"
    echo ""
}

MODE="symlink"
DO_BACKUP=true
CHECK_DEPS=false
WITH_TUICAST=false

while [[ "$#" -gt 0 ]]; do
    case "$1" in
        --symlink) MODE="symlink"; shift ;;
        --copy) MODE="copy"; shift ;;
        --no-backup) DO_BACKUP=false; shift ;;
        --check-deps) CHECK_DEPS=true; shift ;;
        --with-tuicast) WITH_TUICAST=true; shift ;;
        -h|--help) usage; exit 0 ;;
        *) echo -e "${C_RED}Opción desconocida: $1${C_RESET}"; usage; exit 1 ;;
    esac
done

banner

# Step 1: Dependencies check
if [ "$CHECK_DEPS" = true ]; then
    echo -e "${C_BLUE}==> [1/4] Verificando dependencias del sistema...${C_RESET}"
    CORE_PKGS=(
        sway waybar fuzzel mako swaylock foot grim slurp wl-clipboard
        cliphist playerctl brightnessctl libnotify polkit-gnome starship
        btop helix micro zellij cava fastfetch fish adw-gtk-theme
        papirus-icon-theme qt5ct qt6ct xsettingsd networkmanager
        pulsemixer imagemagick chafa ttf-terminus-nerd ttf-meslo-nerd
        gcc make libpulse libsndfile jq
    )

    MISSING=()
    for pkg in "${CORE_PKGS[@]}"; do
        if ! pacman -Qi "$pkg" &>/dev/null; then
            MISSING+=("$pkg")
        fi
    done

    if [ ${#MISSING[@]} -gt 0 ]; then
        echo -e "${C_YELLOW}Paquetes faltantes: ${MISSING[*]}${C_RESET}"
        if command -v yay &>/dev/null; then
            read -rp "¿Deseas instalar los paquetes faltantes con yay? [S/n]: " ans
            if [[ "$ans" =~ ^[sSyY]?$ ]]; then
                yay -S --needed "${MISSING[@]}"
            fi
        fi
    else
        echo -e "${C_GREEN}✓ Todas las dependencias principales están instaladas.${C_RESET}"
    fi

    # Laptop / ThinkPad Hardware Detection
    CHASSIS=$(cat /sys/class/dmi/id/chassis_type 2>/dev/null || echo "")
    IS_LAPTOP=false
    if compgen -G "/sys/class/power_supply/BAT*" >/dev/null || \
       { [ -d /sys/class/backlight ] && [ -n "$(ls -A /sys/class/backlight 2>/dev/null)" ]; } || \
       [[ "$CHASSIS" =~ ^(8|9|10|14|31|32)$ ]]; then
        IS_LAPTOP=true
    fi

    if [ "$IS_LAPTOP" = true ]; then
        echo -e "${C_CYAN}ℹ Detectado chasis tipo Laptop (ThinkPad / Batería encontrada).${C_RESET}"
        LAPTOP_PKGS=(brightnessctl tlp thermald power-profiles-daemon intel-media-driver vulkan-intel sof-firmware bluez bluez-utils)
        LAPTOP_MISSING=()
        for pkg in "${LAPTOP_PKGS[@]}"; do
            if ! pacman -Qi "$pkg" &>/dev/null; then
                LAPTOP_MISSING+=("$pkg")
            fi
        done

        if [ ${#LAPTOP_MISSING[@]} -gt 0 ]; then
            echo -e "${C_YELLOW}Paquetes recomendados para laptop / Intel Tiger Lake faltantes: ${LAPTOP_MISSING[*]}${C_RESET}"
            if command -v yay &>/dev/null; then
                read -rp "¿Deseas instalar los paquetes para laptop con yay? [S/n]: " ans
                if [[ "$ans" =~ ^[sSyY]?$ ]]; then
                    yay -S --needed "${LAPTOP_MISSING[@]}"
                fi
            fi
        else
            echo -e "${C_GREEN}✓ Paquetes optimizados para laptop ya instalados.${C_RESET}"
        fi

        # Power saving services recommendation
        if command -v systemctl &>/dev/null; then
            read -rp "¿Deseas habilitar y arrancar los servicios recomendados de energía (tlp, thermald, bluetooth)? [S/n]: " srv_ans
            if [[ "$srv_ans" =~ ^[sSyY]?$ ]]; then
                echo -e "${C_BLUE}==> Habilitando servicios tlp, thermald y bluetooth...${C_RESET}"
                sudo systemctl enable --now tlp.service thermald.service bluetooth.service 2>/dev/null || true
            fi
        fi
    fi
fi

# Step 2: Ensure destination folders exist
echo -e "${C_BLUE}==> [2/4] Creando directorios destino...${C_RESET}"
mkdir -p "$HOME/.config"
mkdir -p "$HOME/.local/bin"

# Step 3: Backup existing configs
backup_item() {
    local target="$1"
    if [ -e "$target" ] || [ -L "$target" ]; then
        if [ "$DO_BACKUP" = true ]; then
            mkdir -p "$BACKUP_DIR"
            echo -e "${C_YELLOW}  -> Respaldando: $target -> $BACKUP_DIR/${C_RESET}"
            cp -r --parents "$target" "$BACKUP_DIR/" 2>/dev/null || mv "$target" "$BACKUP_DIR/"
        fi
    fi
}

install_item() {
    local src="$1"
    local dest="$2"

    if [ "$MODE" = "symlink" ]; then
        if [ -L "$dest" ] && [ "$(readlink "$dest")" = "$src" ]; then
            echo -e "${C_CYAN}  ✓ Ya enlazado: $dest${C_RESET}"
            return
        fi
        backup_item "$dest"
        rm -rf "$dest"
        ln -sfn "$src" "$dest"
        echo -e "${C_GREEN}  ✓ Enlazado: $dest -> $src${C_RESET}"
    else
        backup_item "$dest"
        rm -rf "$dest"
        cp -r "$src" "$dest"
        echo -e "${C_GREEN}  ✓ Copiado: $src -> $dest${C_RESET}"
    fi
}

echo -e "${C_BLUE}==> [3/4] Instalando dotfiles ($MODE)...${C_RESET}"

# Install .config folders and files
for item in "$DOTFILES_DIR/.config"/*; do
    [ -e "$item" ] || continue
    name="$(basename "$item")"
    install_item "$item" "$HOME/.config/$name"
done

# Install .local/bin scripts
for script in "$DOTFILES_DIR/.local/bin"/*; do
    [ -e "$script" ] || continue
    name="$(basename "$script")"
    chmod +x "$script"
    install_item "$script" "$HOME/.local/bin/$name"
done

# Install home root files (.bashrc, .zshrc, .gtkrc-2.0, etc.)
if [ -d "$DOTFILES_DIR/home" ]; then
    for hfile in "$DOTFILES_DIR/home"/.*; do
        [ -f "$hfile" ] || continue
        name="$(basename "$hfile")"
        [ "$name" = "." ] || [ "$name" = ".." ] && continue
        install_item "$hfile" "$HOME/$name"
    done
fi

# Step 4: Compile Native C Tools (DotWave & SampleDeck)
if [ -d "$DOTFILES_DIR/src/dotwave" ]; then
    echo -e "${C_BLUE}==> Compilando e instalando DotWave (Osciloscopio Braille)...${C_RESET}"
    make -C "$DOTFILES_DIR/src/dotwave" install PREFIX="$HOME/.local/bin" >/dev/null 2>&1 || true
fi

if [ -d "$DOTFILES_DIR/src/sampledeck" ]; then
    echo -e "${C_BLUE}==> Compilando e instalando SampleDeck (Inspector y Analizador de Kicks)...${C_RESET}"
    make -C "$DOTFILES_DIR/src/sampledeck" install PREFIX="$HOME/.local/bin" >/dev/null 2>&1 || true
fi

# Optional: TuiCast (OBS Studio TUI Controller)
if [ "$WITH_TUICAST" = true ]; then
    TUICAST_DIR="${TUICAST_DIR:-$HOME/tuicast}"
    if [ -d "$TUICAST_DIR" ] && command -v cargo >/dev/null 2>&1; then
        echo -e "${C_BLUE}==> Compilando e instalando TuiCast (OBS Studio TUI Controller)...${C_RESET}"
        (cd "$TUICAST_DIR" && cargo build --release && install -m 755 target/release/tuicast "$HOME/.local/bin/tuicast") || {
            echo -e "${C_YELLOW}⚠ No se pudo compilar tuicast automáticamente. Puedes compilarlo con: cd ~/tuicast && cargo build --release${C_RESET}"
        }
        echo -e "${C_CYAN}ℹ  Para habilitar el módulo de estado de OBS en Waybar, descomenta 'custom/arrow-obs' y 'custom/obs' en ~/.config/waybar/config.jsonc${C_RESET}"
    else
        echo -e "${C_YELLOW}ℹ TuiCast no encontrado en $TUICAST_DIR o Rust/cargo no disponible. Saltando.${C_RESET}"
    fi
fi

# Final checks and permissions
chmod +x "$HOME/.local/bin"/* 2>/dev/null || true

echo ""
echo -e "${C_GREEN}╔════════════════════════════════════════════════════════════════╗${C_RESET}"
echo -e "${C_GREEN}║          ¡Instalación de dotfiles completada con éxito!        ║${C_RESET}"
echo -e "${C_GREEN}╚════════════════════════════════════════════════════════════════╝${C_RESET}"
if [ "$DO_BACKUP" = true ] && [ -d "$BACKUP_DIR" ]; then
    echo -e "${C_YELLOW}Copia de seguridad guardada en: $BACKUP_DIR${C_RESET}"
fi
echo -e "${C_CYAN}Para recargar Sway ahora mismo, presiona: Mod+Shift+c${C_RESET}"
echo ""
echo -e "${C_CYAN}ℹ  Para aplicar el tema de login de Ly ejecuta: sudo cp ~/.config/ly/config.ini /etc/ly/config.ini${C_RESET}"
echo ""
