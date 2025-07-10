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
    uint32_t length = 0;
    const uint8_t* value = nullptr;
    FREGetObjectAsUTF8(freObject, &length, &value);
    if (value == nullptr) return "";
    return std::string(reinterpret_cast<const char*>(value), length);
}

uint64_t getUnsignedInt64FromFRE(FREContext ctx, FREObject freObject) {
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
    int32_t value;
    FREResult result = FREGetObjectAsInt32(freObject, &value);

    if (result != FRE_OK) {
        dispatchEvent(ctx, "INT32 Failed", "");
        return 0;
    }

    return value;
}

uint32_t getUnsignedInt32FromFRE(FREContext ctx, FREObject freObject) {
    uint32_t value;
    FREResult result = FREGetObjectAsUint32(freObject, &value);

    if (result != FRE_OK) {
        dispatchEvent(ctx, "UINT32 Failed", "");
        return 0;
    }

    return value;
}

// Get a Discord Activity from FREObject (as AS3 object)
discord::Activity getActivityFromFRE(FREContext ctx, FREObject freObject, discord::Activity* prevActivity) {
    if (prevActivity == nullptr) {
        prevActivity = {};
    }

    // Retrieve activity parameters
    FREObject activityState;
    FREObject activityDetails;
    // For the timestamp
    FREObject activityTimestampStart;
    FREObject activityTimestampEnd;
    // For the activity
    FREObject activityLargeImg;
    FREObject activityLargeImgText;
    FREObject activitySmallImg;
    FREObject activitySmallImgText;
    // For the party
    FREObject activityPartyID;
    FREObject activityPartyCurrentSize;
    FREObject activityPartyMaxSize;
    // For match join secret???
    FREObject activityMatchSecret;
    FREObject activityJoinSecret;
    FREObject activitySpectateSecret;

    // FREGetObjectProperty aaaaaaaaaaaaa
    FREGetObjectProperty(freObject, (const uint8_t*) "state", &activityState, nullptr);
    FREGetObjectProperty(freObject, (const uint8_t*) "details", &activityDetails, nullptr);
    
    FREGetObjectProperty(freObject, (const uint8_t*) "timestampStart", &activityTimestampStart, nullptr);
    FREGetObjectProperty(freObject, (const uint8_t*) "timestampEnd", &activityTimestampEnd, nullptr);
    FREGetObjectProperty(freObject, (const uint8_t*) "largeImage", &activityLargeImg, nullptr);
    FREGetObjectProperty(freObject, (const uint8_t*) "largeText", &activityLargeImgText, nullptr);
    FREGetObjectProperty(freObject, (const uint8_t*) "smallImage", &activitySmallImg, nullptr);
    FREGetObjectProperty(freObject, (const uint8_t*) "smallText", &activitySmallImgText, nullptr);
    FREGetObjectProperty(freObject, (const uint8_t*) "partyID", &activityPartyID, nullptr);
    FREGetObjectProperty(freObject, (const uint8_t*) "partyCurrentSize", &activityPartyCurrentSize, nullptr);
    FREGetObjectProperty(freObject, (const uint8_t*) "partyMaxSize", &activityPartyMaxSize, nullptr);
    FREGetObjectProperty(freObject, (const uint8_t*) "matchSecret", &activityMatchSecret, nullptr);
    FREGetObjectProperty(freObject, (const uint8_t*) "joinSecret", &activityJoinSecret, nullptr);
    FREGetObjectProperty(freObject, (const uint8_t*) "spectateSecret", &activitySpectateSecret, nullptr);

    // Getting correct type to update
    std::string state = getStringFromFRE(ctx, activityState);
    std::string details = getStringFromFRE(ctx, activityDetails);
    dispatchEvent(ctx, "Getting Timestamp Start", "");
    int64_t startTimestamp = getSignedInt64FromFRE(ctx, activityTimestampStart);
    dispatchEvent(ctx, "Getting Timestamp END", "");
    int64_t endTimestamp = getSignedInt64FromFRE(ctx, activityTimestampEnd);
    dispatchEvent(ctx, "Getting Timestamp DONE", "");
    std::string largeImg = getStringFromFRE(ctx, activityLargeImg);
    std::string largeText = getStringFromFRE(ctx, activityLargeImgText);
    std::string smallImg = getStringFromFRE(ctx, activitySmallImg);
    std::string smallText = getStringFromFRE(ctx, activitySmallImgText);
    std::string partyID = getStringFromFRE(ctx, activityPartyID);
    int32_t partyCurrentSize = getInt32FromFRE(ctx, activityPartyCurrentSize);
    int32_t partyMaxSize = getInt32FromFRE(ctx, activityPartyMaxSize);
    std::string matchSecret = getStringFromFRE(ctx, activityMatchSecret);
    std::string joinSecret = getStringFromFRE(ctx, activityJoinSecret);
    std::string spectateSecret = getStringFromFRE(ctx, activitySpectateSecret);

    prevActivity->SetDetails(details.c_str());
    prevActivity->SetState(state.c_str());
    prevActivity->GetTimestamps().SetStart(startTimestamp);
    prevActivity->GetTimestamps().SetEnd(endTimestamp);
    prevActivity->GetAssets().SetSmallImage(smallImg.c_str());
    prevActivity->GetAssets().SetSmallText(smallText.c_str());
    prevActivity->GetAssets().SetLargeImage(largeImg.c_str());
    prevActivity->GetAssets().SetLargeText(largeText.c_str());
    prevActivity->GetParty().GetSize().SetCurrentSize(partyCurrentSize);
    prevActivity->GetParty().GetSize().SetMaxSize(partyMaxSize);
    prevActivity->GetParty().SetId(partyID.c_str());
    prevActivity->GetSecrets().SetJoin(joinSecret.c_str());
    prevActivity->GetSecrets().SetMatch(matchSecret.c_str());
    prevActivity->GetSecrets().SetSpectate(spectateSecret.c_str());

    return *prevActivity;
}

// This is for threading, since RequireDiscord flags causes the C++ to stop the AIR programs


// Init SDK
FREObject initialize(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
    dispatchEvent(ctx, "CORE_INIT", "hey");
    DiscordANEState* state = nullptr;
    FREGetContextNativeData(ctx, (void**) &state);

    // Get parameters from AS3
    uint64_t application_id = getUnsignedInt64FromFRE(ctx, argv[0]);
    uint64_t discordFlag = getUnsignedInt64FromFRE(ctx, argv[1]);
    FREObject activityProp = argv[2];

    dispatchEvent(ctx, "CORE_INIT", "it's me");
    if (!state || state->core) {
        dispatchEvent(ctx, "CORE_INIT", "Huh?");
        return nullptr;
    }

    // Convert string to double
    discord::Core* core_ptr{};
    auto result = discord::Core::Create(application_id, discordFlag, &core_ptr);
    
    dispatchEvent(ctx, "CORE_INIT", "goku");
    if (result != discord::Result::Ok) {
        dispatchEvent(ctx, "CORE_INIT", "Could not connect to Discord.");
        return nullptr;
    }
    dispatchEvent(ctx, "CORE_INIT", "no");
    state->core.reset(core_ptr);
    dispatchEvent(ctx, "CORE_INIT", "way");
    if (!state->core) {
        dispatchEvent(ctx, "CORE_INIT", "Core creation failed.");
        return nullptr;
    }

    // This hook exists for Discord loggings
    state->core->SetLogHook(discord::LogLevel::Debug, [ctx, state](discord::LogLevel level, const char* message) {
        std::cout << "DISCORD_LOG: " << message << std::endl;
        dispatchEvent(ctx, "DISCORD_LOG: ", message + std::to_string(static_cast<int>(level)));
    });

    getActivityFromFRE(ctx, activityProp, &state->activity);

    state->core->ActivityManager().UpdateActivity(state->activity, [ctx, state](discord::Result res) {
        std::string status_message = "ACTIVITY_STATUS: ";
        status_message += (res == discord::Result::Ok) ? "Succeeded" : "Failed";
        dispatchEvent(ctx, "ACTIVITY: ", status_message);
    });
    return nullptr;
}

// running callback
FREObject runCallback(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
    DiscordANEState* state = nullptr;
    FREGetContextNativeData(ctx, (void**)&state);
    if (state && state->core) {
        discord::Result result = state->core->RunCallbacks();
        dispatchEvent(ctx, "RUN_CALLBACK", std::to_string(static_cast<int>(result)));
    }
    else
    {
        dispatchEvent(ctx, "NO_STATE_OR_CORE_FAILED", "");
    }
    return nullptr;
}

// Cập nhật Rich Presence
FREObject updateActivity(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
    DiscordANEState* state = nullptr;
    FREGetContextNativeData(ctx, (void**)&state);
    if (!state || !state->core || argc < 2) return nullptr;

    FREObject props = argv[0];
    
    getActivityFromFRE(ctx, props, &state->activity);
    state->core->ActivityManager().UpdateActivity(state->activity, [ctx](discord::Result result) {
        dispatchEvent(ctx, "HOH??", std::to_string(static_cast<int>(result)));
        if (result == discord::Result::Ok) {
            dispatchEvent(ctx, "ACTIVITY_UPDATE_SUCCESS", "");
        } else {
            dispatchEvent(ctx, "ACTIVITY_UPDATE_FAILED", "");
        }
    });

    return nullptr;
}

// Dọn dẹp tài nguyên
FREObject dispose(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[]) {
    DiscordANEState* state = nullptr;
    FREGetContextNativeData(ctx, (void**)&state);
    if (state && state->core) {
        state->core.reset(); // unique_ptr sẽ tự động gọi destructor và giải phóng
    }
    return nullptr;
}


// Define methods for communicating
void ContextInitializer(void* extData, const uint8_t* ctxType, FREContext ctx, uint32_t* numFunctions, const FRENamedFunction** functions) {
    *numFunctions = 4;
    FRENamedFunction* func = (FRENamedFunction*)malloc(sizeof(FRENamedFunction) * (*numFunctions));
    
    func[0] = { (const uint8_t*)"initialize", nullptr, &initialize };
    func[1] = { (const uint8_t*)"runCallback", nullptr, &runCallback };
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