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
alias update='system-update'
