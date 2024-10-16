#!/bin/bash

function last_login_info() {
    echo "Последний вход пользователей в систему:"
    echo "---------------------------------------"
    lastlog | grep -v "Never" | awk '{print "Пользователь: "$1 "\nПоследний вход: "$4 " "$5 " "$6 " "$7 "\n---------------------------------------"}'
}

function active_sessions_info() {
    echo "Активные сессии пользователей в системе:"
    echo "----------------------------------------"
    who | awk '{print "Пользователь: "$1 "\nТерминал: "$2 "\nВремя входа: "$3 " "$4 "\nIP-адрес: "$5 "\n----------------------------------------"}'
}

function audit_user_logins() {
    echo "Аудит идентификации входов пользователей"
    echo "========================================"
    echo ""
    last_login_info
    echo ""
    active_sessions_info
    echo ""
    echo "Аудит завершен."
}
audit_user_logins
