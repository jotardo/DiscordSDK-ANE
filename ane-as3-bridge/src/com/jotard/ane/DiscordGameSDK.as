package com.jotard.ane
{
    import flash.events.EventDispatcher;
    import flash.external.ExtensionContext;
    import flash.events.StatusEvent;

    public class DiscordGameSDK extends EventDispatcher
    {
        private static var _context:ExtensionContext;
        private static const EXTENSION_ID:String = "com.jotard.ane.DiscordANE";
        private static const NATIVE_HEADER_NAME:String = "[DiscordANE Native]";
        private static const AS3_HEADER_NAME:String = "[DiscordGameSDK]";

        private function onStatus(data:Object):void
        {
            trace(NATIVE_HEADER_NAME, "Code:", data.code, "- Level:", data.level);
            // Solving status, if one want dispatchEvent on this
        }

        public function DiscordGameSDK()
        {
            _context = ExtensionContext.createExtensionContext(EXTENSION_ID, null);
            if (_context) {
                _context.addEventListener(StatusEvent.STATUS, onStatus);
            } else {
                trace(AS3_HEADER_NAME, "ExtensionContext DiscordGameSDK could not be created.");
            }
        }

        public function initialize(applicationID:String, activity:Object=null):void
        {
            if (!_context)
            {
                trace(AS3_HEADER_NAME, "ERROR: No context to call!")
                return;
            }

            var defaultFlag:String = "0";
            var notRequireDiscordFlag:String = "1";
            _context.call("initialize", applicationID, notRequireDiscordFlag, activity);
        }

        public function runCallback():void
        {
            if (!_context)
            {
                trace(AS3_HEADER_NAME, "ERROR: No context to call!")
                return;
            }

            _context.call("runCallback");
        }
        

        public function updateActivity(state:String, details:String, largeImageKey:String, largeImageText:String, smallImageKey:String, smallImageText:String):void
        {
            if (!_context)
            {
                trace(AS3_HEADER_NAME, "ERROR: No context to call!")
                return;
            }

            _context.call("updateActivity", state, details, largeImageKey, largeImageText, smallImageKey, smallImageText);
        }

    }
}