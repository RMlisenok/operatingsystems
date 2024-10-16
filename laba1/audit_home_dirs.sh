#!/bin/bash

HOME_DIRS="/home"

audit_home_dir() {
    local user_dir=$1

    if [ -d "$user_dir" ]; then
        echo "Аудит домашнего каталога: $user_dir"
        echo "----------------------------------"
        local user=$(stat -c '%U' "$user_dir")
        local total_size=$(du -sh "$user_dir" | awk '{print $1}')
        echo "Общий размер каталога: $total_size"
        local file_count=$(find "$user_dir" -type f | wc -l)
        echo "Количество файлов: $file_count"
        local dir_count=$(find "$user_dir" -type d | wc -l)
        echo "Количество подкаталогов: $dir_count"
        local permissions=$(stat -c '%A' "$user_dir")
        echo "Права доступа к каталогу: $permissions"
        echo "----------------------------------"
        echo ""
    else
        echo "Каталог $user_dir не найден."
    fi
}

audit_all_home_dirs() {
    echo "Аудит всех домашних каталогов в $HOME_DIRS"
    echo ""
    for dir in "$HOME_DIRS"/*; do
        audit_home_dir "$dir"
    done

    echo "Аудит завершен."
}
audit_all_home_dirs
