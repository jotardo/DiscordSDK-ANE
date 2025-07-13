package
{
	import flash.display.Sprite;
	import flash.display.StageAlign;
	import flash.display.StageScaleMode;
	import flash.text.TextField;
	import flash.text.TextFieldAutoSize;
	import flash.text.TextFormat;
	import com.jotard.ane.DiscordGameSDK;
	import flash.utils.Timer;
	import flash.events.TimerEvent;

	public class testapplication extends Sprite
	{

		public function testapplication()
		{
			super();

			stage.scaleMode = StageScaleMode.NO_SCALE;
			stage.align = StageAlign.TOP_LEFT;

			var textField:TextField = new TextField();
			textField.autoSize = TextFieldAutoSize.LEFT;
			textField.defaultTextFormat = new TextFormat("_sans", 12, 0x0000cc);
			textField.text = "Hello, Adobe AIR!";
			addChild(textField);

			var currentDate:Date = new Date();
			var unixTimestamp:Number = Math.floor(currentDate.time / 1000);
			var unixTimestamp2:Number = Math.floor(currentDate.time / 1000) + 60;
			trace(unixTimestamp, unixTimestamp2);
			
			trace("A");
			var sdk:DiscordGameSDK = new DiscordGameSDK();
			trace("B");
			sdk.initialize("424087019149328395", {
				"details":"bruh",
				"state":"HUUUH?"
			});
			trace("C");
			var timer:Timer = new Timer(1000);
			timer.addEventListener(TimerEvent.TIMER, function (e:*):void {
				sdk.runCallbacks();
			});
			timer.start();
		}
	}
}
