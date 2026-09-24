if test -f /usr/share/cachyos-zsh-config/cachyos-config.zsh; then
    source /usr/share/cachyos-zsh-config/cachyos-config.zsh
fi

if command -v eza &>/dev/null; then
    alias ls='eza -lh --icons --group-directories-first'
    alias l='eza -lh --icons --group-directories-first'
    alias ll='eza -lha --icons --group-directories-first'
    alias la='eza -a --icons --group-directories-first'
    alias lla='eza -lha --icons --group-directories-first'
    alias tree='eza --tree --icons'
else
    alias ls='ls --color=auto -lh'
    alias l='ls --color=auto -lh'
    alias ll='ls --color=auto -lha'
    alias la='ls --color=auto -la'
    alias lla='ls --color=auto -lha'
fi

export PATH="$HOME/.local/bin:$PATH"
export EDITOR=micro
export VISUAL=micro

# SSH Agent Socket
if [ -z "$SSH_AUTH_SOCK" ] && [ -n "$XDG_RUNTIME_DIR" ] && [ -S "$XDG_RUNTIME_DIR/ssh-agent.socket" ]; then
    export SSH_AUTH_SOCK="$XDG_RUNTIME_DIR/ssh-agent.socket"
fi

# Quick navigation aliases
alias ..='cd ..'
alias ...='cd ../..'
alias ....='cd ../../..'

# Modern CLI tools
if command -v bat &>/dev/null; then
    alias cat='bat --paging=never'
    alias bcat='bat'
fi

# TUI & System Aliases
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
alias update='system-update'
alias wave='dotwave'
alias sdeck='sampledeck'
alias sampleview='sampledeck'
alias dots='git -C "$HOME/dotfiles"'
alias power='fuzzel-powerprofile'
alias tp-bat='thinkpad-battery'
alias vol='volume-tool'
alias bright='brightness-tool'
alias menu='fuzzel-dashboard'
alias dash='fuzzel-dashboard'
alias keys='tui-keybindings'
alias atajos='tui-keybindings'

# Starship Prompt integration
if command -v starship &>/dev/null; then
    eval "$(starship init zsh)"
fi

# Zoxide smart jump integration
if command -v zoxide &>/dev/null; then
    eval "$(zoxide init zsh)"
fi
