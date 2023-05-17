/**
 * @file ICMS.h
 * @author Anton Stapenhorst (anton@stapenhorst.org)
 * @brief Inter Core Messaging System
 * @version 0.1
 * @date 2023-05-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */



#pragma once

class ICMS
{
public:
    static ICMS& Instance();
    ~ICMS(){};
private:
    ICMS(){};
    ICMS(const ICMS&) = delete;
    ICMS(ICMS&&) = delete;
    ICMS & operator=(const ICMS&) = delete;
    ICMS & operator=(ICMS&&) = delete;

private:
    int buffer[10];
};

