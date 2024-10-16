#!/bin/bash

function audit_processes() {
    echo "Аудит состояния процессов"
    echo "=========================="
    echo ""
    printf "%-10s %-20s %-10s %-10s %-10s %-10s\n" "PID" "Пользователь" "%CPU" "%MEM" "Статус" "Команда"
    echo "------------------------------------------------------------------------------------"
    ps -eo pid,user,%cpu,%mem,stat,comm --sort=-%cpu | head -n 15 | tail -n 14 | awk '{printf "%-10s %-20s %-10s %-10s %-10s %-10s\n", $1, $2, $3, $4, $5, $6}'
}

function run_audit() {
    echo "Запуск аудита состояния процессов..."
    echo ""
    audit_processes
    echo ""
    echo "Аудит завершен."
}
run_audit
