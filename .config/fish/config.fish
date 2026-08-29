# ==============================================================================
# Fish Shell Configuration - Defqon.1 Industrial TUI Edition
# ==============================================================================

# Source CachyOS base fish config (includes all CachyOS aliases and functions)
if test -f /usr/share/cachyos-fish-config/cachyos-config.fish
    source /usr/share/cachyos-fish-config/cachyos-config.fish
end

# Disable fastfetch / greeting at startup
function fish_greeting
end
set -g fish_greeting ""

# Environment variables
set -gx PATH "$HOME/.local/bin" $PATH
set -gx EDITOR micro
set -gx VISUAL micro
set -gx TERMINAL foot
set -gx BROWSER cachy-browser
set -gx GTK_THEME "adw-gtk3-dark"
set -gx ADW_DEBUG_COLOR_SCHEME "prefer-dark"
set -gx QT_STYLE_OVERRIDE "Oxygen"
set -gx QT_QPA_PLATFORMTHEME "qt6ct"

# Modern Colored Line-by-Line LS / Eza Aliases
if command -v eza >/dev/null 2>&1
    alias ls='eza -lh --icons --group-directories-first'
    alias l='eza -lh --icons --group-directories-first'
    alias ll='eza -lha --icons --group-directories-first'
    alias la='eza -a --icons --group-directories-first'
    alias lla='eza -lha --icons --group-directories-first'
    alias lt='eza --tree --icons --level=2'
    alias tree='eza --tree --icons'
else
    alias ls='ls --color=auto -lh'
    alias l='ls --color=auto -lh'
    alias ll='ls --color=auto -lha'
    alias la='ls --color=auto -la'
    alias lla='ls --color=auto -lha'
end

if command -v bat >/dev/null 2>&1
    alias cat='bat --paging=never'
    alias bcat='bat'
end

alias lg='lazygit'
alias y='yazi'
alias top='btop'
alias mix='pulsemixer'
alias bt='bluetui'
alias wifi='fuzzel-wifi'
alias zj='zellij'
alias pkg='tui-packages'
alias in='tui-packages'
alias cal='tui-calendar'
alias wave='dotwave'

# Quick navigation aliases
alias ..='cd ..'
alias ...='cd ../..'
alias ....='cd ../../..'

# Starship Prompt integration
if command -v starship >/dev/null 2>&1
    starship init fish | source
end

# Zoxide smart jump integration
if command -v zoxide >/dev/null 2>&1
    zoxide init fish | source
end
alias update='system-update'
alias sdeck='sampledeck'
alias sampleview='sampledeck'
