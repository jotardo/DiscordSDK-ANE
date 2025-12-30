package com.jotard.ane
{
    import flash.events.EventDispatcher;
    import flash.external.ExtensionContext;

    public class DiscordGameSDK extends EventDispatcher 
    {

        private static const EXTENSION_ID:String = "com.jotard.ane.DiscordANE";
        private static const NATIVE_HEADER_NAME:String = "[DiscordANENative]";
        private static const AS3_HEADER_NAME:String = "[DiscordGameSDK]";

        private static var _context:ExtensionContext;

        public function DiscordGameSDK()
        {
            _context = ExtensionContext.createExtensionContext("com.jotard.ane.DiscordANE", null);
            if (_context)
            {
                _context.addEventListener("status", onStatus);
            }
            else
            {
                (trace("[DiscordGameSDK]", "ExtensionContext DiscordGameSDK could not be created."));
            };
        }

        private function onStatus(data:Object):void
        {
            (trace("[DiscordANENative]", "Code:", data.code, "- Level:", data.level));
            if ((((data.level is String) || (data.level is Number)) || (data.level is Boolean)))
            {
                this.dispatchEvent(new DiscordEvent(data.code, data.level));
            }
            else
            {
                this.dispatchEvent(new DiscordEvent(data.code, JSON.parse(data.level)));
            };
        }

        public function initialize(applicationID:String, activity:Object=null, reconnectInterval:int=5000):void
        {
            var applicationID = applicationID;
            var activity = activity;
            var reconnectInterval = reconnectInterval;
            if ((!(_context)))
            {
                (trace("[DiscordGameSDK]", "ERROR: No context to call!"));
                return;
            };
            var defaultFlag:String = "0";
            var notRequireDiscordFlag:String = "1";
            _context.call("initialize", applicationID, notRequireDiscordFlag, activity);
            var onSuccessConnect:Function = function (data:Object):void
            {
                var statusCode:int;
                _context.removeEventListener("status", onSuccessConnect);
                if (((data) && (data.code == "CORE_INIT")))
                {
                    statusCode = data.level;
                    if ((statusCode == 0))
                    {
                        _context.call("updateActivity", activity);
                    };
                };
            };
            _context.addEventListener("status", onSuccessConnect);
        }

        public function runCallbacks():void
        {
            if ((!(_context)))
            {
                (trace("[DiscordGameSDK]", "ERROR: No context to call!"));
                return;
            };
            _context.call("runCallbacks");
        }

        public function updateActivity(activity:Object):void
        {
            if ((!(_context)))
            {
                (trace("[DiscordGameSDK]", "ERROR: No context to call!"));
                return;
            };
            _context.call("updateActivity", activity);
        }

        public function sendRequestReply(userID:String, replyNum:int):void
        {
            if ((!(_context)))
            {
                (trace("[DiscordGameSDK]", "ERROR: No context to call!"));
                return;
            };
            _context.call("sendRequestReply", userID, replyNum);
        }

        public function sendInvite(userID:String, activationType:int, content:String):void
        {
            if ((!(_context)))
            {
                (trace("[DiscordGameSDK]", "ERROR: No context to call!"));
                return;
            };
            _context.call("sendInvite", userID, activationType, content);
        }

        public function acceptInvite(userID:String):void
        {
            if ((!(_context)))
            {
                (trace("[DiscordGameSDK]", "ERROR: No context to call!"));
                return;
            };
            _context.call("acceptInvite", userID);
        }


    }
}//package com.jotard.ane