#pragma once

#include <afx.h>

#include <iostream>
#include <memory>
#include <thread>
#include <mutex>

#include <map>

//JSON Header
#include <rapidjson.h>
#include <document.h>
#include <writer.h>
#include <reader.h>
#include <stringbuffer.h>
#include <stringbuffer.h>
#include <filewritestream.h>
#include <prettywriter.h>

#include <curl/curl.h>

#include "CppSQLite3.h"

static const char* Wallet_DB_Name = "Wallet.db";
static const char* Reward_DB_Name = "Reward.db";
static const char* Result_DB_Name = "Result.db";

typedef enum eDB_Type : unsigned int
{
    DB_WALLET = 0,
    DB_NETWORK,
    DB_PROJECT,
    DB_REWARD,
    DB_RESULT,
    DB_END
}DB_Type;

typedef enum eFile_Type : unsigned int
{
	FILE_TYPE_REWARD = 0,
    FILE_TYPE_RESULT,
    FILE_TYPE_END
}File_Type;

#pragma pack(push, 1) 
typedef struct StErrorCode
{
    CString Code;
    CString Msg;
    CString Hex;

    StErrorCode()
    {
        Code.Empty();
        Msg.Empty();
        Hex.Empty();
    }
}ErrorCode;

typedef struct StReward_Result
{
    CString Wallet_Address;
    CString TransactionHash;
    CString ErrorCode;
    CString ErrorMsg;

    long double	Total_Reward_Coin;

    long double	Price;
    __int64		Work_Time;

    bool		bState;

    StReward_Result()
    {
        Total_Reward_Coin = 0.0f;
        Price = 0.0f;
        Work_Time = 0;
        Wallet_Address.Empty();
        TransactionHash.Empty();
        ErrorCode.Empty();
        ErrorMsg.Empty();
        bState = false;
    }
}Reward_Result;

typedef struct StReward_Info
{    
    long double Price;
    __int64 Work_Time;
    
    CString	 Wallet_addr;
    CString  Time_Unit;
    CString  Project_ID;
    CString  SubTask_ID;

    StReward_Info()
    {
        Price = 0.0f;
        Work_Time = 0;

        Time_Unit.Empty();
        Wallet_addr.Empty();
        Project_ID.Empty();
        SubTask_ID.Empty();
    }
}Reward_Info;

typedef struct StRewardList
{
    CString Wallet_Address;
    CString TxHash;
    CString Price;
    CString ErrorCode;
    CString ErrorMsg;

    StRewardList()
    {
        Wallet_Address.Empty();
        TxHash.Empty();
        Price.Empty();
        ErrorCode.Empty();
        ErrorMsg.Empty();
    }
}RewardList;

typedef struct StSumReward_Info
{
    long double Price;
    CString	 Wallet_addr;

    StSumReward_Info()
    {
        Price = 0.0f;
        Wallet_addr.Empty();
    }
}SumReward_Info;

typedef struct StReward
{    
    CString  Project_Flag;	// RUNNING or DONE 	
    CString  Master_Wallet_Addr;
    CString  Master_Wallet_Privatekey;

    std::multimap<CString, struct StReward_Info> Reward_List;

    StReward()
    {
        Project_Flag.Empty();
        Master_Wallet_Addr.Empty();
        Reward_List.clear();
    }
}Reward;

//토큰 재전송을 위해 사용하는 구조체
typedef struct StReReward
{
    CString  Master_Wallet_Addr;
    CString  Master_Wallet_Privatekey;

    std::map<CString, StRewardList> ReReward_List;

    StReReward()
    {
        Master_Wallet_Addr.Empty();
        Master_Wallet_Privatekey.Empty();
        ReReward_List.clear();
    }
}ReReward;

typedef struct stBalanceInfo
{
    long double Balance;

    stBalanceInfo()
    {
        Balance = 0.0f;
    }
}BalanceInfo;

typedef struct stReward_Merge
{
    CString szWallet_Address;
    long double szTotal_Token;
    CString szGas_Price;
    CString szGas_Limit;

    stReward_Merge()
    {
        szWallet_Address.Empty();
        szTotal_Token = 0.0f;
        szGas_Price.Empty();
        szGas_Limit.Empty();
    }
}Reward_Merge;

#pragma pack(pop)