package com.jotard.ane
{
    import flash.events.EventDispatcher;
    import flash.external.ExtensionContext;
    import flash.events.StatusEvent;

    public class DiscordGameSDK extends EventDispatcher
    {
        private static var _context:ExtensionContext;
        private static const EXTENSION_ID:String = "com.jotard.ane.DiscordANE";
        private static const NATIVE_HEADER_NAME:String = "[DiscordANENative]";
        private static const AS3_HEADER_NAME:String = "[DiscordGameSDK]";

        private function onStatus(data:Object):void
        {
            trace(NATIVE_HEADER_NAME, "Code:", data.code, "- Level:", data.level);
        }

        public function DiscordGameSDK()
        {
            _context = ExtensionContext.createExtensionContext(EXTENSION_ID, null);
            if (_context)
            {
                _context.addEventListener(StatusEvent.STATUS, onStatus);
            }
            else
            {
                trace(AS3_HEADER_NAME, "ExtensionContext DiscordGameSDK could not be created.");
            }
        }

        public function initialize(applicationID:String, activity:Object=null, reconnectInterval:int = 5000):void
        {
            if (!_context)
            {
                trace(AS3_HEADER_NAME, "ERROR: No context to call!")
                return;
            }

            var defaultFlag:String = "0";
            var notRequireDiscordFlag:String = "1";
            _context.call("initialize", applicationID, notRequireDiscordFlag, activity);

            var onSuccessConnect:Function;
            onSuccessConnect = function(data:Object):void
            {
                _context.removeEventListener(StatusEvent.STATUS, onSuccessConnect);
                if (data && data.code == "CORE_INIT")
                {
                    var statusCode:int = int(data.level);
                    if (statusCode == DiscordResult.Ok)
                    {
                        _context.call("updateActivity", activity);
                    }
                };
            }
            _context.addEventListener(StatusEvent.STATUS, onSuccessConnect);
        }

        public function runCallbacks():void
        {
            if (!_context)
            {
                trace(AS3_HEADER_NAME, "ERROR: No context to call!")
                return;
            }

            _context.call("runCallbacks");
        }

        public function updateActivity(activity:Object):void
        {
            if (!_context)
            {
                trace(AS3_HEADER_NAME, "ERROR: No context to call!")
                return;
            }

            _context.call("updateActivity", activity);
        }

        public function sendRequestReply(userID:String, replyNum:int):void
        {
            if (!_context)
            {
                trace(AS3_HEADER_NAME, "ERROR: No context to call!")
                return;
            }

            _context.call("sendRequestReply", userID, replyNum);
        }

        public function sendInvite(userID:String, activationType:int, content:String):void
        {
            if (!_context)
            {
                trace(AS3_HEADER_NAME, "ERROR: No context to call!")
                return;
            }

            _context.call("sendInvite", userID, activationType, content);
        }

        public function acceptInvite(userID:String):void
        {
            if (!_context)
            {
                trace(AS3_HEADER_NAME, "ERROR: No context to call!")
                return;
            }

            _context.call("acceptInvite", userID);
        }

    }
}