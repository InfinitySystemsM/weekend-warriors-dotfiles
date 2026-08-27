#
# ~/.bashrc
#

# If not running interactively, don't do anything
[[ $- != *i* ]] && return

# Modern Colored LS / Eza Aliases
if command -v eza &>/dev/null; then
    alias ls='eza --icons --group-directories-first'
    alias l='eza -l --icons --group-directories-first'
    alias ll='eza -lh --icons --group-directories-first'
    alias la='eza -a --icons --group-directories-first'
    alias lla='eza -lha --icons --group-directories-first'
    alias tree='eza --tree --icons'
else
    alias ls='ls --color=auto'
    alias ll='ls --color=auto -lh'
    alias la='ls --color=auto -la'
fi

alias grep='grep --color=auto'
PS1='[\u@\h \W]\$ '

export PATH="$HOME/.local/bin:$PATH"
