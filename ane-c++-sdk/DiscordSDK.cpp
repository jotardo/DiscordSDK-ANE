#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "discord.h"
#include "FlashRuntimeExtensions.h"

// Sending Event back to the AS3
void dispatchEvent(FREContext ctx, const std::string& code, const std::string& level = "") {
    FREDispatchStatusEventAsync(ctx, (const uint8_t*)code.c_str(), (const uint8_t*)level.c_str());
}

// Global state for handling
struct DiscordANEState {
    std::unique_ptr<discord::Core> core;
    discord::Activity activity;
};

// Get a String from FREObject
std::string getStringFromFRE(FREContext ctx, FREObject freObject) {
    if (freObject == nullptr) return nullptr;
    uint32_t length = 0;
    const uint8_t* value = nullptr;
    FREGetObjectAsUTF8(freObject, &length, &value);
    if (value == nullptr) return nullptr;
    return std::string(reinterpret_cast<const char*>(value), length);
}

uint64_t getUnsignedInt64FromFRE(FREContext ctx, FREObject freObject) {
    if (freObject == nullptr) return 0;
    // Lấy FREObject dưới dạng chuỗi UTF8
    uint32_t length = 0;
    const uint8_t* value = nullptr;
    FREResult result = FREGetObjectAsUTF8(freObject, &length, &value);

    if (result != FRE_OK || value == nullptr || length == 0) {
        dispatchEvent(ctx, "UINT64 Failed", "");
        return 0;
    }

    // Chuyển đổi chuỗi C-style thành std::string
    std::string strValue(reinterpret_cast<const char*>(value), length);

    // Sử dụng try-catch để xử lý các lỗi chuyển đổi và ném ra ngoại lệ
    try {
        // Dùng std::stoull (string to unsigned long long) để phân tích và trả về chuỗi
        return std::stoull(strValue);
    } catch (const std::exception& e) {
        // Ném ra một ngoại lệ mới với thông điệp lỗi rõ ràng hơn
        throw std::runtime_error("Failed to convert string to uint64_t: " + std::string(e.what()));
    }
}

int64_t getSignedInt64FromFRE(FREContext ctx, FREObject freObject) {
    if (freObject == nullptr) return 0;
    uint32_t length = 0;
    const uint8_t* value = nullptr;
    FREResult result = FREGetObjectAsUTF8(freObject, &length, &value);
    
    if (result != FRE_OK || value == nullptr || length == 0) {
        dispatchEvent(ctx, "INT64 Failed", "");
        return 0;
    }
    
    std::string strValue(reinterpret_cast<const char*>(value), length);
    
    try {
        // Dùng std::stoll (string to long long) để phân tích và trả về chuỗi
        return std::stoll(strValue);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to convert string to int64_t: " + std::string(e.what()));
    }
}

int32_t getInt32FromFRE(FREContext ctx, FREObject freObject) {
    if (freObject == nullptr) return 0;
    int32_t value;
    FREResult result = FREGetObjectAsInt32(freObject, &value);

    if (result != FRE_OK) {
        dispatchEvent(ctx, "INT32 Failed", "");
        return 0;
    }

    return value;
}

uint32_t getUnsignedInt32FromFRE(FREContext ctx, FREObject freObject) {
    if (freObject == nullptr) return 0;
    uint32_t value;
    FREResult result = FREGetObjectAsUint32(freObject, &value);

    if (result != FRE_OK) {
        dispatchEvent(ctx, "UINT32 Failed", "");
        return 0;
    }

    return value;
}

FREObject getObjectPropertyFromFRE(FREContext ctx, FREObject freObject, std::string propName) {
    FREObject object = nullptr;
    FREResult result = FREGetObjectProperty(freObject, (const uint8_t*) propName.c_str(), &object, nullptr);
    if (result == FRE_OK) {
        FREObjectType objectType;
        FREGetObjectType(object, &objectType);
        if (objectType != FRE_TYPE_NULL) {
            return object;
        }
    }
    return nullptr;
}


// Get a Discord Activity from FREObject (as AS3 object)
void getActivityFromFRE(FREContext ctx, FREObject freObject, discord::Activity* activityToUpdate) {
    // Luôn kiểm tra con trỏ đầu vào để đảm bảo an toàn.
    if (activityToUpdate == nullptr || freObject == nullptr) {
        dispatchEvent(ctx, "pointersucks", "");
        return;
    }

    FREObject prop = nullptr;

    // --- State & Details ---
    prop = getObjectPropertyFromFRE(ctx, freObject, "state");
    if (prop) activityToUpdate->SetState(getStringFromFRE(ctx, prop).c_str());

    prop = getObjectPropertyFromFRE(ctx, freObject, "details");
    if (prop) activityToUpdate->SetDetails(getStringFromFRE(ctx, prop).c_str());

    // --- Timestamps ---
    prop = getObjectPropertyFromFRE(ctx, freObject, "timestampStart");
    if (prop) activityToUpdate->GetTimestamps().SetStart(getSignedInt64FromFRE(ctx, prop));
    
    prop = getObjectPropertyFromFRE(ctx, freObject, "timestampEnd");
    if (prop) activityToUpdate->GetTimestamps().SetEnd(getSignedInt64FromFRE(ctx, prop));

    // --- Assets ---
    prop = getObjectPropertyFromFRE(ctx, freObject, "largeImage");
    if (prop) activityToUpdate->GetAssets().SetLargeImage(getStringFromFRE(ctx, prop).c_str());

    prop = getObjectPropertyFromFRE(ctx, freObject, "largeText");
    if (prop) activityToUpdate->GetAssets().SetLargeText(getStringFromFRE(ctx, prop).c_str());

    prop = getObjectPropertyFromFRE(ctx, freObject, "smallImage");
    if (prop) activityToUpdate->GetAssets().SetSmallImage(getStringFromFRE(ctx, prop).c_str());
    
    prop = getObjectPropertyFromFRE(ctx, freObject, "smallText");
    if (prop) activityToUpdate->GetAssets().SetSmallText(getStringFromFRE(ctx, prop).c_str());
    
    // --- Party ---
    prop = getObjectPropertyFromFRE(ctx, freObject, "partyID");
    if (prop) activityToUpdate->GetParty().SetId(getStringFromFRE(ctx, prop).c_str());
    
    prop = getObjectPropertyFromFRE(ctx, freObject, "partyCurrentSize");
    if (prop) activityToUpdate->GetParty().GetSize().SetCurrentSize(getInt32FromFRE(ctx, prop));
    
    prop = getObjectPropertyFromFRE(ctx, freObject, "partyMaxSize");
    if (prop) activityToUpdate->GetParty().GetSize().SetMaxSize(getInt32FromFRE(ctx, prop));

    // --- Secrets ---
    prop = getObjectPropertyFromFRE(ctx, freObject, "matchSecret");
    if (prop) activityToUpdate->GetSecrets().SetMatch(getStringFromFRE(ctx, prop).c_str());

    prop = getObjectPropertyFromFRE(ctx, freObject, "joinSecret");
    if (prop) activityToUpdate->GetSecrets().SetJoin(getStringFromFRE(ctx, prop).c_str());

    prop = getObjectPropertyFromFRE(ctx, freObject, "spectateSecret");
    if (prop) activityToUpdate->GetSecrets().SetSpectate(getStringFromFRE(ctx, prop).c_str());
}

// initate connection to the SDK and Return DiscordStatus
FREObject initialize(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
    DiscordANEState* state = nullptr;
    FREGetContextNativeData(ctx, (void**) &state);

    // Get parameters from AS3
    uint64_t application_id = getUnsignedInt64FromFRE(ctx, argv[0]);
    uint64_t discordFlag = getUnsignedInt64FromFRE(ctx, argv[1]);

    if (state && state->core) {
        dispatchEvent(ctx, "CORE_INIT", std::to_string(uint32_t(discord::Result::Ok)));
        return nullptr;
    }

    // Convert string to double
    discord::Core* core_ptr{};
    auto result = discord::Core::Create(application_id, discordFlag, &core_ptr);
    
    if (result != discord::Result::Ok) {
        dispatchEvent(ctx, "CORE_INIT", std::to_string(uint32_t(result)));
        return nullptr;
    }
    state->core.reset(core_ptr);

    // This hook exists for Discord loggings
    state->core->SetLogHook(discord::LogLevel::Debug, [ctx, state](discord::LogLevel level, const char* message) {
        dispatchEvent(ctx, "DISCORD_LOG", std::string(message));
    });
    
    dispatchEvent(ctx, "CORE_INIT", std::to_string(uint32_t(result)));
    return nullptr;
}

// running callback, return 
FREObject runCallbacks(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
    DiscordANEState* state = nullptr;
    FREGetContextNativeData(ctx, (void**)&state);
    if (state && state->core) {
        discord::Result result = state->core->RunCallbacks();
        dispatchEvent(ctx, "RUN_CALLBACKS", std::to_string(uint32_t(result)));
    }
    else
    {
        dispatchEvent(ctx, "RUN_CALLBACKS", std::to_string(uint32_t(discord::Result::NotRunning)));
    }
    return nullptr;
}

// Cập nhật Rich Presence
FREObject updateActivity(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
    DiscordANEState* state = nullptr;
    FREGetContextNativeData(ctx, (void**)&state);
    if (argc < 1) {
        dispatchEvent(ctx, "UPDATE_ACTIVITY", std::to_string(uint32_t(discord::Result::InvalidPayload)));
        return nullptr;
    }
    if (!state || !state->core) {
        dispatchEvent(ctx, "UPDATE_ACTIVITY", std::to_string(uint32_t(discord::Result::NotFound)));
        return nullptr;
    }

    FREObject props = argv[0];
    discord::Activity newActivity = state->activity;
    getActivityFromFRE(ctx, props, &newActivity);
    state->core->ActivityManager().UpdateActivity(newActivity, [ctx](discord::Result result) {
        dispatchEvent(ctx, "UPDATE_ACTIVITY", std::to_string(uint32_t(result)));
    });

    return nullptr;
}

// Cleaning up. Depends on your needs
FREObject dispose(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
    DiscordANEState* state = nullptr;
    FREGetContextNativeData(ctx, (void**)&state);
    if (state && state->core) {
        state -> core.reset();
    }
    return nullptr;
}


// Define methods for communicating
void ContextInitializer(void* extData, const uint8_t* ctxType, FREContext ctx, uint32_t* numFunctions, const FRENamedFunction** functions) {
    *numFunctions = 4;
    FRENamedFunction* func = (FRENamedFunction*)malloc(sizeof(FRENamedFunction) * (*numFunctions));
    
    func[0] = { (const uint8_t*)"initialize", nullptr, &initialize };
    func[1] = { (const uint8_t*)"runCallbacks", nullptr, &runCallbacks };
    func[2] = { (const uint8_t*)"updateActivity", nullptr, &updateActivity };
    func[3] = { (const uint8_t*)"dispose", nullptr, &dispose };
    
    *functions = func;
    
    DiscordANEState* state = new DiscordANEState();
    FRESetContextNativeData(ctx, state);
}

void ContextFinalizer(FREContext ctx) {
    DiscordANEState* state = nullptr;
    FREGetContextNativeData(ctx, (void**)&state);
    if (state) {
        delete state;
        FRESetContextNativeData(ctx, nullptr);
    }
}

extern "C" {
    __declspec(dllexport) void DiscordGameSDKExtensionInitializer(void** extData, FREContextInitializer* ctxInitializer, FREContextFinalizer* ctxFinalizer) {
        *ctxInitializer = &ContextInitializer;
        *ctxFinalizer = &ContextFinalizer;
    }

    __declspec(dllexport) void DiscordGameSDKExtensionFinalizer(void* extData) {
        return;
    }
}