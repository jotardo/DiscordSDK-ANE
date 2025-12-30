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
	import flash.display.SimpleButton;
	import flash.text.TextInteractionMode;
	import flash.text.TextDisplayMode;
	import flash.text.TextFieldType;
	import flash.display.Shape;
	import flash.events.MouseEvent;

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
			sdk.initialize("1365007682200670302", {
				"details":"bruh",
				"state":"HUUUH?",
				"partyID": "sssaw",
				"partyCurrentSize":1,
				"partyMaxSize":10,
				"joinSecret": "xdx1234",
				"instance": true
			});
			trace("C");
			var timer:Timer = new Timer(1000);
			timer.addEventListener(TimerEvent.TIMER, function (e:*):void {
				sdk.runCallbacks();
			});
			timer.start();

			var textField2:TextField = new TextField();
			textField2.type = TextFieldType.INPUT;
			textField2.y = 40;
			textField2.border = true;
			textField2.multiline = true;
			textField2.wordWrap = true;
			addChild(textField2);

			var upSprite:Sprite=new Sprite();
			upSprite.graphics.lineStyle(2,0,1);
			upSprite.graphics.drawRect(0,0,50,20);
			var hitSprite:Shape=new Shape();
			hitSprite.graphics.beginFill(0,1);
			hitSprite.graphics.drawRect(3,3,44,14); // a bit smaller rect to make the button narrow
			var button:SimpleButton=new SimpleButton(upSprite,upSprite,upSprite,hitSprite);
			button.x = 100;
			button.y = 40;
			button.addEventListener(MouseEvent.CLICK, function(e):void
			{
				sdk.sendInvite(textField2.text, 1, "HEYYA");
			});
			addChild(button);
		}
	}
}
