#pragma once
#ifndef SECURITY_OPERATIONS_H
#define SECURITY_OPERATIONS_H

#include <string>

void DisplayFilePermissions(const std::string& filename);
void ModifyFilePermissions(const std::string& filename);

#endif // SECURITY_OPERATIONS_H

